add_subdirectory(util)
add_subdirectory(game)

if(BUILD_MINMAX_STRATEGY OR BUILD_RANDOM_STRATEGY)
  add_subdirectory(strategies)
endif()

if(BUILD_UI)
  add_subdirectory(ui)
endif()

add_subdirectory(benchmarks)
