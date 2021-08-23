# Register our executable
add_clang_executable(uqcs_demo
    Main.cpp
    )

# Link the LLVM support libraries into our binary
set(LLVM_LINK_COMPONENTS support)

# Link the Libtooling libraries as well
target_link_libraries(uqcs_demo
    PRIVATE
    clangTooling
    clangBasic
    clangASTMatchers
    )
