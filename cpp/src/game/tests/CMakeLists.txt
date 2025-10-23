file(GLOB _sources
    CONFIGURE_DEPENDS
    "*.cpp"
    "*.c"
    "*.inl"
    "*.h"
    "*.hpp"
)

add_executable(Alphalcazar.Game.Tests ${_sources})
target_link_libraries(Alphalcazar.Game.Tests Alphalcazar.Game gtest::gtest)
gtest_discover_tests(Alphalcazar.Game.Tests)
