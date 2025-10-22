# C++17 implementation

This folder contains the C++17 implementation of the project, which is the currently mantained version.

## Setting up the project

### Required software

- Microsoft Visual Studio Community 2019/2022
- [CMake 3.24.0](https://cmake.org/download) or above
- [Python 3.10](https://www.python.org/downloads/) or above (to install conan)
- Latest Conan 2.x version (via `pip install "conan<3"`)

### Generate the project solution

The project solution for MSVS is generated with CMake and its dependencies are installed with Conan.

After conan's first install, setup your default profile:

```bash
conan profile detect
```

For MSVS, conan does sometimes not guess the correct C++ standard version. Navigate to your default conan profile
(location can be found by running `conan profile path default`) and make sure the `compiler.cppstd` option is set to `17`.

Run the following command in your console at the root folder of the project to install the dependencies:

```bash
conan install . -s build_type=Release --build=missing --output-folder=build
conan install . -s build_type=Debug --build=missing --output-folder=build
```

Then generate the project solution with CMake:

```bash
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
```

This will generate the `alphalcazar.sln` in the newly created `build` folder, which you can open with Visual Studio.
