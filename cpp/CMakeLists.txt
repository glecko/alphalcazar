cmake_minimum_required(VERSION 3.23)

# Project
project(alphalcazar CXX)

# Include conan cmake
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

# Set vs-specific config
if(CMAKE_GENERATOR MATCHES "^Visual Studio.*$")
  set(vstest TRUE)
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
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.16.1/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake"
                EXPECTED_HASH SHA256=396e16d0f5eabdc6a14afddbcfff62a54a7ee75c6da23f32f7a31bc85db23484
                TLS_VERIFY ON)
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)
conan_check(VERSION 1.43.0 REQUIRED)
conan_cmake_configure(GENERATORS cmake_find_package_multi)
conan_cmake_run(CONANFILE "conanfile.txt" BASIC_SETUP CMAKE_TARGETS BUILD missing BUILD_TYPE CMAKE_TARGETS)

# Enable GoogleTest to discover tests
if(BUILD_TESTS)
  enable_testing()
  include(GoogleTest)
endif()

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Alphalcazar)

# Subdirectories
add_subdirectory(src)
