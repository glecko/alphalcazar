file(GLOB_RECURSE _sources
    CONFIGURE_DEPENDS
    "src/*.cpp"
    "src/*.inl"
    "src/*.hpp"
    "include/*.inl"
    "include/*.hpp"
)

add_library(Alphalcazar.Strategy.Random STATIC ${_sources})
set_target_properties(Alphalcazar.Strategy.Random PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS true)
target_include_directories(Alphalcazar.Strategy.Random PUBLIC include/)

target_link_libraries(Alphalcazar.Strategy.Random Alphalcazar.Game)

if (BUILD_TESTS)
  add_subdirectory(tests)
endif()
