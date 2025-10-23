file(GLOB _sources
    CONFIGURE_DEPENDS
    "*.cpp"
    "*.c"
    "*.inl"
    "*.h"
    "*.hpp"
)

add_executable(Alphalcazar.Utils.Tests ${_sources})
target_link_libraries(Alphalcazar.Utils.Tests Alphalcazar.Utils gtest::gtest)
gtest_discover_tests(Alphalcazar.Utils.Tests)
