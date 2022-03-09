file(GLOB _sources
    CONFIGURE_DEPENDS
    "*.cpp"
    "*.c"
    "*.inl"
    "*.h"
    "*.hpp"
)

add_library(Alphalcazar.Game STATIC ${_sources})
set_property(TARGET Alphalcazar.Game PROPERTY POSITION_INDEPENDENT_CODE ON)
set_property(TARGET Alphalcazar.Game PROPERTY CXX_VISIBILITY_PRESET default)
set_target_properties(Alphalcazar.Game PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(Alphalcazar.Game PUBLIC ${CMAKE_CURRENT_LIST_DIR} "${SHARED_DIR}")

target_link_libraries(Alphalcazar.Game PUBLIC Alphalcazar.Utils)

if (BUILD_TESTS)
  add_subdirectory(tests)
endif()
