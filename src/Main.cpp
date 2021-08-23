#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "llvm/Support/CommandLine.h"

#include <fstream>

using namespace llvm;
using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;

// ----------------------------------------------------------------------------
// COMMAND LINE ARGUMENT SETUP
// ----------------------------------------------------------------------------

// Set up the standard command line arguments for the tool.
// These command line arguments are standard across all tools
// built using the LLVM framework.
static cl::OptionCategory uqcsDemoCategory("uqcs-demo options");
static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);

// Here we add an extra non-standard command line flag 
// for demonstration purposes
static cl::opt<bool> verbose("verbose",
    cl::desc(R"(Generate verbose output)"),     // the description
    cl::init(false),                            // the initial value of the option
    cl::cat(uqcsDemoCategory)                   // what category this belongs to
);

// ----------------------------------------------------------------------------
// DEFAULT WARNING SUPPRESSION
// ----------------------------------------------------------------------------

// The WarningDiagConsumer allows us to suppress warning and error messages
// which are raised when a file is being parsed by clang. This allows us to
// turn off extraneous output since we assume that we are checking code 
// which already compiles.
class WarningDiagConsumer : public DiagnosticConsumer {

    public:
    virtual void HandleDiagnostic(
            DiagnosticsEngine::Level Level, const Diagnostic& Info) override {
        // simply do nothing
    }
};

// ----------------------------------------------------------------------------
// CALLBACK CLASSES
// ----------------------------------------------------------------------------

// Here we will be adding our callback classes which will be
// doing the actual code analysis
//
// ...

// ----------------------------------------------------------------------------
// REGISTERING CALLBACKS
// ----------------------------------------------------------------------------

// The ASTConsumer allows us to dictate:
//      - which AST nodes we match, and
//      - how we want to handle those matched nodes
class UQCSDemoASTConsumer : public ASTConsumer {

    public:
    UQCSDemoASTConsumer(clang::Preprocessor& PP) {
    
        // Here we add all of the checks that should be run
        // when the AST is traversed by using Matcher.addMatcher

        // At the moment, there are no checks registered
    }

    void HandleTranslationUnit(ASTContext& Context) override {
        Matcher.matchAST(Context);
    }

    private:
    MatchFinder Matcher;
};

// The FrontEndAction is the main entry point for the clang tooling library
// and allows us to add callbacks via:
//      PPCallback classes  - for callbacks involving the preprocessor
//      ASTConsumer classes - for callbacks involving AST nodes 
class UQCSDemoFrontEndAction : public ASTFrontendAction {

    public:
    virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(
            CompilerInstance& CI, StringRef file) override {

        // Here we add any checks which require the preprocessor.
        // At the moment, there are no checks registered

        return std::unique_ptr<ASTConsumer>(
                new UQCSDemoASTConsumer(CI.getPreprocessor()));
    }
};

// ----------------------------------------------------------------------------
// OUR PROGRAM
// ----------------------------------------------------------------------------

// Checks if the given filepath can be accessed without issue.
// Returns true if it is accessible, else returns false.
bool filepathAccessible(std::string path)
{   
    std::ifstream file(path);
    return file.is_open();
}

int main(int argc, const char** argv)
{
    // Parse the command line arguments. This will provide us with
    // the list of files we need to check as well as allow us to
    // check for the optional flags. Note that we also allow for
    // zero or more arguments to allow for more fine-grained error
    // checking
    auto OptionsParser = CommonOptionsParser::create(argc, argv, uqcsDemoCategory, cl::ZeroOrMore);
    if (auto err = OptionsParser.takeError()) {
        llvm::errs() << std::move(err);
        return EXIT_FAILURE;
    }

    // Our program is meant to analyse source code, so if we didn't
    // get any filepaths, we print an error message and exit
    auto files = OptionsParser->getSourcePathList();
    if (files.empty()) {
        llvm::errs() << "Error: No input files specified\n";
        return EXIT_FAILURE;
    }

    // Also, if any of the filepaths we've received are invalid,
    // we also print an error message and exit
    for (auto path : files) {
        if (!filepathAccessible(path)) {
            llvm::errs() << "Unable to access file '" << path << "'\n";
            return EXIT_FAILURE;
        }
    }

    // Next, we create the tool which will perform all of the 
    // code analysis. In the base code you are provided, this
    // tool doesn't perform any analysis at all.
    ClangTool Tool(OptionsParser->getCompilations(), files);
    Tool.setDiagnosticConsumer(new WarningDiagConsumer);
    Tool.run(newFrontendActionFactory<UQCSDemoFrontEndAction>().get());

    return EXIT_SUCCESS;
}