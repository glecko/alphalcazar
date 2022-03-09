cmake_minimum_required(VERSION 3.16.0)

# Project
project(alphalcazar CXX)

# Set vs-specific config
if(CMAKE_GENERATOR MATCHES "^Visual Studio.*$")
  set(vstest TRUE)
endif()

# Build options
option(BUILD_MINMAX_STRATEGY "Build minmax strategy" ON)
option(BUILD_RANDOM_STRATEGY "Build random strategy" ON)
option(BUILD_TESTS "Compile tests" ON)

# Include conan cmake
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

# Put output files in /bin
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/bin/$<CONFIG>")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/bin/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/bin/$<CONFIG>")

# Set C++ standard and require extensions
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED)
set(CMAKE_CXX_EXTENSIONS ON)

# install libraries with conan
# -------------
# First, fetch conan.cmake from the remote repo
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.16.1/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake"
                EXPECTED_HASH SHA256=396e16d0f5eabdc6a14afddbcfff62a54a7ee75c6da23f32f7a31bc85db23484
                TLS_VERIFY ON)
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)
conan_config_install(ITEM ${CMAKE_CURRENT_LIST_DIR}/conan_config/)
conan_check(VERSION 1.23.0 REQUIRED)
conan_cmake_run(CONANFILE "conanfile.txt" BASIC_SETUP CMAKE_TARGETS BUILD missing BUILD_TYPE CMAKE_TARGETS)

# Enable GOogleTest to discover tests
if(BUILD_TESTS)
  enable_testing()
  include(GoogleTest)
endif()

# Set visual studio compile options
if(MSVC)
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
  set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
  set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
  set(CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CMAKE_STATIC_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")

  # https://docs.microsoft.com/en-us/cpp/build/reference/compiler-option-warning-level?view=vs-2019
  add_compile_options(/W4)
  # https://docs.microsoft.com/en-us/cpp/build/reference/permissive-standards-conformance?view=vs-2019
  add_compile_options(/permissive-)
  # https://docs.microsoft.com/en-us/cpp/build/reference/utf-8-set-source-and-executable-character-sets-to-utf-8?view=vs-2019
  add_compile_options(/utf-8)
  add_compile_options(/arch:AVX)

  # https://docs.microsoft.com/en-us/cpp/build/reference/sdl-enable-additional-security-checks?view=vs-2019
  #add_compile_options(/sdl)
  # https://docs.microsoft.com/en-us/cpp/build/reference/analyze-code-analysis?view=vs-2019
  #add_compile_options(/analyze:only)
  # https://docs.microsoft.com/en-us/cpp/build/reference/gl-whole-program-optimization?view=vs-2019
  #add_compile_options(/GL)
  # https://docs.microsoft.com/en-us/cpp/build/reference/experimental-preprocessor?view=vs-2019
  #add_compile_options(/experimental:preprocessor)
  # https://docs.microsoft.com/en-us/cpp/build/reference/guard-enable-control-flow-guard?view=vs-2019
  #add_compile_options(/guard)

  # Warnings as errors
  add_compile_options(/WX)
endif()

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Alphalcazar)

# Subdirectories
add_subdirectory(src)
