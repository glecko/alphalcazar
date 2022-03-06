file(GLOB _sources
    CONFIGURE_DEPENDS
    "*.cpp"
    "*.c"
    "*.inl"
    "*.h"
    "*.hpp"
)

add_library(Alphalcazar.Strategy.MinMax STATIC ${_sources})
set_property(TARGET Alphalcazar.Strategy.MinMax PROPERTY CXX_VISIBILITY_PRESET default)
set_target_properties(Alphalcazar.Strategy.MinMax PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(Alphalcazar.Strategy.MinMax PUBLIC ${CMAKE_CURRENT_LIST_DIR} "${SHARED_DIR}")

target_link_libraries(Alphalcazar.Strategy.MinMax Alphalcazar.Game)

if (BUILD_TESTS)
  add_subdirectory(tests)
endif()
