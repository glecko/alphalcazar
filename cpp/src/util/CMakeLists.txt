file(GLOB_RECURSE _sources
    CONFIGURE_DEPENDS
    "src/*.cpp"
    "src/*.inl"
    "src/*.hpp"
    "include/*.inl"
    "include/*.hpp"
)

add_library(Alphalcazar.Utils STATIC ${_sources})
set_target_properties(Alphalcazar.Utils PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS true)
target_include_directories(Alphalcazar.Utils PUBLIC include/)

find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
target_link_libraries(Alphalcazar.Utils PUBLIC spdlog::spdlog fmt::fmt)

if (BUILD_TESTS)
  add_subdirectory(tests)
endif()