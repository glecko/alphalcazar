file(GLOB_RECURSE _sources
    CONFIGURE_DEPENDS
    "src/*.cpp"
    "src/*.inl"
    "src/*.hpp"
    "include/*.inl"
    "include/*.h"
    "include/*.hpp"
)

add_library(Alphalcazar.Game STATIC ${_sources})
set_target_properties(Alphalcazar.Game PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS true)
set_property(TARGET Alphalcazar.Game PROPERTY CXX_VISIBILITY_PRESET default)
target_include_directories(Alphalcazar.Game PUBLIC include/)
set_target_properties(Alphalcazar.Game PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(Alphalcazar.Game PUBLIC Alphalcazar.Utils)

if (BUILD_TESTS)
  add_subdirectory(tests)
endif()
