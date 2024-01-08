cmake_minimum_required(VERSION 3.9)

# Project
project(alphalcazar CXX)

# Include conan cmake
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

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

# Install depencencies with conan
# This code has been fetched from conan cmake's README: https://github.com/conan-io/cmake-conan
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake"
                TLS_VERIFY ON)
endif()

function(configure_conan_dependencies generator)
  if(BUILD_TESTS)
    conan_cmake_configure(REQUIRES gtest/1.10.0 spdlog/[>=1.4.1] GENERATORS ${generator})
  else()
    conan_cmake_configure(REQUIRES spdlog/[>=1.4.1] GENERATORS ${generator})
  endif()
  # Build spdlog without exceptions as we want exception handling to be disabled
  add_definitions(-DSPDLOG_NO_EXCEPTIONS=1)
endfunction()

include(${CMAKE_BINARY_DIR}/conan.cmake)
conan_check(VERSION 1.49.0 REQUIRED)
# If the generator is multi config (ex. MSVS) install conan dependencies
# with the cmake_find_package_multi generator for each of the CMAKE_CONFIGURATION_TYPES.
# For single-config generators (ex. Ninja on CI), use the cmake_find_package generator.
get_property(multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(multi_config)
  configure_conan_dependencies(cmake_find_package_multi)
  foreach(TYPE ${CMAKE_CONFIGURATION_TYPES})
      message("Installing conan dependencies for configuration type '${TYPE}'...")
      conan_cmake_autodetect(settings BUILD_TYPE ${TYPE})
      conan_cmake_install(PATH_OR_REFERENCE . BUILD missing SETTINGS ${settings})
  endforeach()
else()
  message("Installing conan dependencies for single configuration type '${CMAKE_BUILD_TYPE}'...")
  configure_conan_dependencies(cmake_find_package)
  conan_cmake_autodetect(settings)
  conan_cmake_install(PATH_OR_REFERENCE . BUILD missing SETTINGS ${settings})
endif()

# Compiler-specific configuration
if(MSVC)
  message("Setting compile options for MSVS (compiler id: '${CMAKE_CXX_COMPILER_ID}')...")
  # Warning level 4 and all warnings as errors
  add_compile_options(/W4)
  add_compile_options(/WX)

  # The current (and subsequent) versions of CMake hardcode the /EHsc (c++ exception handling) and /GR (C++ RTTI)
  # flags for MSVS, which forces us to do this string replacement hack in order to disable them:
  # https://gitlab.kitware.com/cmake/cmake/blob/v3.9.1/Modules/Platform/Windows-MSVC.cmake#L151

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
