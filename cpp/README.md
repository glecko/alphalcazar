# C++17 implementation

This folder contains the C++17 implementation of the project, which is the currently mantained version.

## Setting up the project

### Required software

- Microsoft Visual Studio Community 2019
- CMake 3.23.0
- Python 3.10 (to install conan)
- Conan (via `pip install conan`)

### Generate the project solution

The project solution for MSVS is generated with CMake. Run the following commands in your console at the root folder of the project:

```bash
mkdir build
cd build
cmake ..
```

This will generate the `alphalcazar.sln` in the newly created `build` folder, which you can open with Visual Studio.
