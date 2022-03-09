# Dummy executable that links into Alphalcazar.Game
add_executable(Alphalcazar main.cpp)
target_link_libraries(Alphalcazar Alphalcazar.Game Alphalcazar.Strategy.MinMax Alphalcazar.Utils)
add_dependencies(Alphalcazar Alphalcazar.Game Alphalcazar.Strategy.MinMax Alphalcazar.Utils)
