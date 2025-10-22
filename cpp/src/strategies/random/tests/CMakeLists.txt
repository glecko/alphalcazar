file(GLOB _sources
    CONFIGURE_DEPENDS
    "*.cpp"
    "*.c"
    "*.inl"
    "*.h"
    "*.hpp"
)

add_executable(Alphalcazar.Strategy.Random.Tests ${_sources})
target_link_libraries(Alphalcazar.Strategy.Random.Tests Alphalcazar.Game Alphalcazar.Strategy.Random gtest::gtest)
gtest_discover_tests(Alphalcazar.Strategy.Random.Tests)
