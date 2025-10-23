file(GLOB_RECURSE _sources
    CONFIGURE_DEPENDS
    "src/*.cpp"
    "src/*.inl"
    "src/*.hpp"
    "include/*.inl"
    "include/*.hpp"
)

add_library(Alphalcazar.Strategy.MinMax STATIC ${_sources})
set_target_properties(Alphalcazar.Strategy.MinMax PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS true)
target_include_directories(Alphalcazar.Strategy.MinMax PUBLIC include/)

target_link_libraries(Alphalcazar.Strategy.MinMax Alphalcazar.Game Alphalcazar.Utils)

if (BUILD_TESTS)
  add_subdirectory(tests)
endif()
