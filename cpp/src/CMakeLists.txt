set(SHARED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shared")

# Subdirectories
add_subdirectory(shared)
add_subdirectory(game)

if(BUILD_MINMAX_STRATEGY)
  add_subdirectory(strategies)
endif()

add_subdirectory(main)
