add_executable(Alphalcazar.Benchmark.MinMaxStrategy main.cpp)
target_link_libraries(Alphalcazar.Benchmark.MinMaxStrategy Alphalcazar.Game Alphalcazar.Strategy.MinMax Alphalcazar.Utils)
add_dependencies(Alphalcazar.Benchmark.MinMaxStrategy Alphalcazar.Game Alphalcazar.Strategy.MinMax Alphalcazar.Utils)
