# UQCS Tech Talk - An Intro to Code Analysis with LLVM

Welcome to the repo! Here we have the base code
to get you started with creating a basic code analysis
tool using LLVM and its Libtooling framework.

## Quick Setup

To generate the required boilerplate structure and 
install the necessary dependencies, simply run

```
./setup.sh
```

In order to use the script, you need to have either the `apt`
or `brew` package managers installed on your system.

> Note that for some Linux distributions the version of `cmake` 
    that will be installed by the package managers is insufficient
    to build the LLVM libraries. In this case, you will need to
    install an updated version of `cmake` yourself - details about
    this can be found [here](https://cmake.org/install/).

## Manual Setup
If you would like to manually set up the development environment, do the following:

1. Install the necessary build tools (`git`, `cmake` and `ninja`)

    ```
    sudo apt install git cmake ninja-build
    ```
    ```
    sudo brew install git cmake ninja-build
    ```

2. Clone the LLVM repository 

    ```
    git clone \
        https://github.com/llvm/llvm-project.git \
        --branch llvmorg-12.0.1
    ```

3. Create a new project directory under 
    `llvm-project/clang-tools-extra` and copy
    the provided source code into it

    ```
    cp -r \
        ./src \
        ./llvm-project/clang-tools-extra/uqcs_demo
    ```

4. Register the project under
    `llvm-project/clang-tools-extra/CMakeLists.txt`

    ```
    echo -e "\nadd_subdirectory(uqcs_demo)\n" >> \
        ./llvm-project/clang-tools-extra/CMakeLists.txt
    ```

5. Create a new `build` directory and move into it
    ```
    mkdir ./llvm-project/build 
    cd ./llvm-project/build
    ```

5. Run the `cmake` initialisation sequence

    ```
    cmake -G Ninja ../llvm \
        -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" \
        -DCMAKE_BUILD_TYPE:STRING=Release
    ```

6. Build the project (from within the `build` directory)

    ```
    ninja uqcs_demo
    ```

## Locating the executable
When the project is built, the resulting executable (`uqcs_demo`)
will be located under `llvm-project/build/bin`.

## Rebuilding the project
When you've made changes to the source code under 
`llvm-project/clang-tools-extra/uqcs_demo` you can rebuild
the final executable by running the following from the 
`build` directory:

```
ninja uqcs_demo
```