cmake_minimum_required(VERSION 3.24)

set(CMAKE_PROJECT_TOP_LEVEL_INCLUDES, ${CMAKE_CURRENT_LIST_DIR}/cmake_external/conan_provider.cmake)

# Project
project(alphalcazar CXX)

# Set vs-specific config
if(CMAKE_GENERATOR MATCHES "^Visual Studio.*$")
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Alphalcazar.Benchmark.MinMaxStrategy)
endif()

# Build options
option(BUILD_MINMAX_STRATEGY "Build minmax strategy" ON)
option(BUILD_RANDOM_STRATEGY "Build random strategy" ON)
option(BUILD_TESTS "Compile tests" ON)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED)


# Compiler-specific configuration
if(MSVC)
  message("Setting compile options for MSVS (compiler id: '${CMAKE_CXX_COMPILER_ID}')...")
  # Warning level 4 and all warnings as errors
  add_compile_options(/W4)
  add_compile_options(/WX)

  # The current (and subsequent) versions of CMake hardcode the /EHsc (c++ exception handling) and /GR (C++ RTTI)
  # flags for MSVS, which forces us to do this string replacement hack in order to disable them:
  # https://gitlab.kitware.com/cmake/cmake/blob/v3.24.0/Modules/Platform/Windows-MSVC.cmake#L194

  # Disable exception handling as this project does not make use of them
  string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

  # Make sure exceptions are disabled in the stdlib implementation of MSVS
  add_definitions(-D_HAS_EXCEPTIONS=0)

  # Disable the C++ RTTI as this project does not need this feature
  string(REPLACE "/GR" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
elseif (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  # The following compile options are supported both by Clang++ and GCC
  message("Setting compile options for Clang/GCC (compiler id: '${CMAKE_CXX_COMPILER_ID}')...")

  # All warning levels and all warnings as errors
  add_compile_options(-Wall)
  add_compile_options(-Wextra)
  add_compile_options(-Werror)

  # Disable exception handling as this project does not make use of them
  add_compile_options(-fno-exceptions)

  # Disable the C++ RTTI as this project does not need this feature
  add_compile_options(-fno-rtti)
endif()

# Enable GoogleTest to discover tests
if(BUILD_TESTS)
  find_package(GTest REQUIRED)
  enable_testing()
  include(GoogleTest)
endif()

# Subdirectories
add_subdirectory(src)
