set(SHARED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shared")

add_subdirectory(shared)
add_subdirectory(game)

if(BUILD_MINMAX_STRATEGY OR BUILD_RANDOM_STRATEGY)
  add_subdirectory(strategies)
endif()

add_subdirectory(main)
