# C++17 implementation

This folder contains the C++17 implementation of the project, which is the currently mantained version.

## Setting up the project

### Prerequisites

- Microsoft Visual Studio Community 2019/2022
- [CMake 3.23.0](https://cmake.org/download) or above
- [Python 3.10](https://www.python.org/downloads/) or above (to install conan)
- Latest Conan 1.x version (via `pip install "conan<2"`)

### Optional prerequisites

- To build the UI (CMake option `-DBUILD_UI=TRUE`), you need to install the [Vulkan SDK (v1.3.268.0)](https://vulkan.lunarg.com/sdk/home#windows)

### Generate the project solution

The project solution for MSVS is generated with CMake. Run the following commands in your console at the root folder of the project:

```bash
mkdir build
cd build
cmake ..
```

This will generate the `alphalcazar.sln` in the newly created `build` folder, which you can open with Visual Studio.
