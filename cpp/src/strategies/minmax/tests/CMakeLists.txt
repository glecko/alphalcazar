file(GLOB _sources
    CONFIGURE_DEPENDS
    "*.cpp"
    "*.c"
    "*.inl"
    "*.h"
    "*.hpp"
)

add_executable(Alphalcazar.Strategy.MinMax.Tests ${_sources})
target_link_libraries(Alphalcazar.Strategy.MinMax.Tests Alphalcazar.Game Alphalcazar.Strategy.MinMax gtest::gtest)
gtest_discover_tests(Alphalcazar.Strategy.MinMax.Tests)
