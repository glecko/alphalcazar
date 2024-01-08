add_subdirectory(util)
add_subdirectory(game)

if(BUILD_MINMAX_STRATEGY OR BUILD_RANDOM_STRATEGY)
  add_subdirectory(strategies)
endif()

add_subdirectory(benchmarks)
