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

add_library(Alphalcazar.Strategy.Random STATIC ${_sources})
set_target_properties(Alphalcazar.Strategy.Random PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS true)
set_property(TARGET Alphalcazar.Strategy.Random PROPERTY CXX_VISIBILITY_PRESET default)
target_include_directories(Alphalcazar.Strategy.Random PUBLIC include/)

target_include_directories(Alphalcazar.Strategy.Random PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(Alphalcazar.Strategy.Random Alphalcazar.Game)

if (BUILD_TESTS)
  add_subdirectory(tests)
endif()
