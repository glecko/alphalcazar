file(GLOB_RECURSE _sources
    CONFIGURE_DEPENDS
    "src/*.cpp"
    "src/*.inl"
    "src/*.hpp"
    "include/*.inl"
    "include/*.hpp"
)

add_library(Alphalcazar.Game STATIC ${_sources})
set_target_properties(Alphalcazar.Game PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS true)
target_include_directories(Alphalcazar.Game PUBLIC include/)

find_package(fmt REQUIRED)
target_link_libraries(Alphalcazar.Game PUBLIC Alphalcazar.Utils fmt::fmt)

if (BUILD_TESTS)
  add_subdirectory(tests)
endif()
