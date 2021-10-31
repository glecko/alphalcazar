# Dummy executable that links into Alphalcazar.Game
add_executable(Alphalcazar main.cpp)
target_link_libraries(Alphalcazar Alphalcazar.Game)
add_dependencies(Alphalcazar Alphalcazar.Game)
