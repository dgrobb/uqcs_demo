#!/usr/bin/env bash
#
# setup.sh
#
# Does the required setup to download the LLVM
# libraries and set up the project for the demo

SCRIPT_PATH="$( cd "$(dirname "$0")" > /dev/null 2>&1; pwd -P )"

# Check that we have the necessary package managers
echo "[+] Checking for suitable package managers"
if command -v apt &> /dev/null
then 
    PACKAGE_MGR=apt
elif command -v brew &> /dev/null
then
    PACKAGE_MGR=brew
else
    PACKAGE_MGR=unknown
fi

if [ "${PACKAGE_MGR}" == unknown ]; then
    echo "    -- Unable to find 'apt' or 'brew'. Please install one of these package managers first"
    echo "    -- Aborting setup"
    exit 1
fi
echo "    -- Found package manager: ${PACKAGE_MGR}"

# Once we know a package manager exists, we attempt to 
# install cmake and ninja if they are not installed already
echo "[+] Checking for build tools"

REQUIRED_BUILD_TOOLS=(git cmake ninja)
MISSING_BUILD_TOOLS=()

for tool in "${REQUIRED_BUILD_TOOLS[@]}"; do
    if ! command -v "${tool}" &> /dev/null
    then
        echo "    -- Unable to find '${tool}'"

        if [ "${tool}" == "ninja" ]; then
            tool+=-build
        fi
        MISSING_BUILD_TOOLS+=("${tool}")
    fi
done

if [ ${#MISSING_BUILD_TOOLS[@]} -gt 0 ]; then
    echo "    -- Running '${PACKAGE_MGR} update'"
    sudo ${PACKAGE_MGR} update
    if [ $? -ne 0 ]; then
        "    -- Aborting setup"
        exit 1
    fi

    echo "    -- Installing missing packages"
    sudo ${PACKAGE_MGR} install "${MISSING_BUILD_TOOLS[@]}"
    if [ $? -ne 0 ]; then
        "    -- Aborting setup"
        exit 1
    fi
else
    echo "    -- All required build tools are already installed"
fi

# Next, we attempt to clone the llvm repository if it doesn't already exist
cd "${SCRIPT_PATH}"
echo "[+] Checking for LLVM source code"
if [ -d "${SCRIPT_PATH}/llvm-project" ]; then
    echo "    -- A copy of the LLVM source code has already been downloaded"
else
    echo "    -- Downloading the LLVM libraries into ${SCRIPT_PATH}/llvm-project. Note: This will take a while"
    git clone https://github.com/llvm/llvm-project.git --branch llvmorg-12.0.1

    if [ $? -ne 0 ]; then 
        echo "    -- An error occurred while checking out the LLVM libraries"
        echo "    -- Aborting setup"
        rm -rf llvm-project
        exit 1
    fi
fi

# Once we have the files, we create our project directory
# and register it in llvm-project/clang-tools-extra/CMakeLists.txt
# if we haven't done so already
echo "[+] Creating project directory structure"

if [ -d "${SCRIPT_PATH}/llvm-project/clang-tools-extra/uqcs_demo" ]; then
    echo "    -- A project directory has already been created"
else
    echo "    -- Creating directory ${SCRIPT_PATH}/llvm-project/clang-tools-extra/uqcs_demo"
    cp -r ./src llvm-project/clang-tools-extra/uqcs_demo
    if [ $? -ne 0 ]; then 
        echo "    -- An error occurred while creating the project directory"
        echo "    -- Aborting setup"
        exit 1
    fi
fi

grep uqcs_demo ${SCRIPT_PATH}/llvm-project/clang-tools-extra/CMakeLists.txt &> /dev/null
if [ $? -eq 0 ]; then
    echo "    -- The project directory has already been registered in llvm-project/clang-tools-extra/CMakeLists.txt"
else
    echo "    -- Registering project directory in llvm-project/clang-tools-extra/CMakeLists.txt"

    echo -e "\nadd_subdirectory(uqcs_demo)\n" >> llvm-project/clang-tools-extra/CMakeLists.txt
    if [ $? -ne 0 ]; then 
        echo "    -- An error occurred while registering the project directory"
        echo "    -- Aborting setup"
        exit 1
    fi
fi

# Now that everything's in place, we build the project
cd llvm-project
echo "[+] Building the project"

if [ ! -d ${SCRIPT_PATH}/llvm-project/build ]; then
    echo "    -- Creating build directory ${SCRIPT_PATH}/llvm-project/build"
    mkdir build

    if [ $? -ne 0 ]; then 
        echo "    -- An error occurred while creating the build directory"
        echo "    -- Aborting setup"
        exit 1
    fi
fi

echo "    -- Moving into directory ${SCRIPT_PATH}/llvm-project/build"
cd build

# Run the cmake initialisation
echo "    -- Running cmake initialisation sequence"
cmake -G Ninja ../llvm \
    -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" \
    -DCMAKE_BUILD_TYPE:STRING=Release

if [ $? -ne 0 ]; then
    echo "    -- Something went wrong while running cmake"
    echo "    -- Aborting setup"
    exit 1
fi

# Build the project
echo "    -- Building the project"
ninja uqcs_demo

if [ $? -ne 0 ]; then
    echo "    -- Something went wrong while building the project"
    echo "    -- Aborting setup"
    exit 1
fi

