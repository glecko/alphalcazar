# Alphalcazar

C++17 implementation of the advanced tic tac toe board game, invented by a friend of mine (Adrià Balcazar), together with several implementations of an AI for said game.

## Rules

The rules of the game can be found in the [rules document](docs/RULES.md).

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
