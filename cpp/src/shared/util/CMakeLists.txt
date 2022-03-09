file(GLOB_RECURSE _sources
    CONFIGURE_DEPENDS
    "*.cpp"
    "*.c"
    "*.inl"
    "*.h"
    "*.hpp"
)

add_library(Alphalcazar.Utils SHARED ${_sources})
set_target_properties(Alphalcazar.Utils PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS true)
set_property(TARGET Alphalcazar.Utils PROPERTY CXX_VISIBILITY_PRESET default)
set_target_properties(Alphalcazar.Utils PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(Alphalcazar.Utils PUBLIC ${SHARED_DIR})

include(CMakePrintHelpers)
cmake_print_variables(CONAN_INCLUDE_DIRS_SPDLOG)
cmake_print_variables(CONAN_COMPILE_DEFINITIONS_SPDLOG)
cmake_print_variables(CONAN_LIB_DIRS_SPDLOG)
cmake_print_variables(CONAN_LIBS_SPDLOG)

target_include_directories(Alphalcazar.Utils SYSTEM PUBLIC ${CONAN_INCLUDE_DIRS_SPDLOG})
target_compile_definitions(Alphalcazar.Utils PUBLIC ${CONAN_COMPILE_DEFINITIONS_SPDLOG})
target_link_directories(Alphalcazar.Utils PUBLIC ${CONAN_LIB_DIRS_SPDLOG})

target_link_libraries(Alphalcazar.Utils PUBLIC ${CONAN_LIBS_SPDLOG} CONAN_PKG::spdlog)
