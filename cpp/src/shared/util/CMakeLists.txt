file(GLOB_RECURSE _sources
    CONFIGURE_DEPENDS
    "*.cpp"
    "*.c"
    "*.inl"
    "*.h"
    "*.hpp"
)

add_library(Alphalcazar.Utils SHARED ${_sources})
set_property(TARGET Alphalcazar.Utils PROPERTY POSITION_INDEPENDENT_CODE ON)
set_property(TARGET Alphalcazar.Utils PROPERTY CXX_VISIBILITY_PRESET default)
set_target_properties(Alphalcazar.Utils PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(Alphalcazar.Utils PUBLIC ${SHARED_DIR})
