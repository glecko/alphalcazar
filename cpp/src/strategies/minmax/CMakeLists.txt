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

add_library(Alphalcazar.Strategy.MinMax STATIC ${_sources})
set_target_properties(Alphalcazar.Strategy.MinMax PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS true)
set_property(TARGET Alphalcazar.Strategy.MinMax PROPERTY CXX_VISIBILITY_PRESET default)
target_include_directories(Alphalcazar.Strategy.MinMax PUBLIC include/)
set_target_properties(Alphalcazar.Strategy.MinMax PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(Alphalcazar.Strategy.MinMax Alphalcazar.Game Alphalcazar.Utils)

if (BUILD_TESTS)
  add_subdirectory(tests)
endif()
