file(GLOB_RECURSE _sources
    CONFIGURE_DEPENDS
    "src/*.cpp"
    "src/*.c"
    "src/*.inl"
    "src/*.h"
    "src/*.hpp"
    "include/*.inl"
    "include/*.h"
    "include/*.hpp"
)

add_library(Alphalcazar.Utils SHARED ${_sources})
set_target_properties(Alphalcazar.Utils PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS true)
set_property(TARGET Alphalcazar.Utils PROPERTY CXX_VISIBILITY_PRESET default)
target_include_directories(Alphalcazar.Utils PUBLIC include/)
set_target_properties(Alphalcazar.Utils PROPERTIES LINKER_LANGUAGE CXX)

find_package(spdlog REQUIRED)
target_link_libraries(Alphalcazar.Utils PUBLIC ${CONAN_LIBS_SPDLOG} spdlog::spdlog)
