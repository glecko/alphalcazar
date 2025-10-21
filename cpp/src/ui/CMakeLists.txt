# Setup a imgui GLFW+Vulkan backend library, based on ImGui's example on:
# https://github.com/ocornut/imgui/blob/master/examples/example_glfw_vulkan

add_library(imgui_backend STATIC external/imgui_backend/imgui_impl_vulkan.cpp external/imgui_backend/imgui_impl_glfw.cpp)

find_package(imgui REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)

target_link_libraries(imgui_backend PUBLIC glfw imgui::imgui Vulkan::Vulkan)
target_include_directories(imgui_backend SYSTEM PUBLIC external/imgui_backend)

# Setup the UI executable using the ImGui backend defined above

file(GLOB_RECURSE _sources
    CONFIGURE_DEPENDS
    "src/*.cpp"
    "src/*.inl"
    "src/*.hpp"
)

add_executable(Alphalcazar.Ui main.cpp ${_sources})
target_link_libraries(Alphalcazar.Ui Alphalcazar.Game Alphalcazar.Utils imgui::imgui imgui_backend)
add_dependencies(Alphalcazar.Ui Alphalcazar.Game Alphalcazar.Utils)

# Set the debugger working directory to the executable output directory, to ensure that
# relative paths behave identically when debugging and when shipping the executable
set_property(TARGET Alphalcazar.Ui PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:Alphalcazar.Ui>)

# Add a post-build custom command to copy the "resources" folder (images, fonts...) to the target folder
ADD_CUSTOM_COMMAND(
    TARGET Alphalcazar.Ui POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/resources" "$<TARGET_FILE_DIR:Alphalcazar.Ui>/resources"
    COMMENT "Copying UI resources to target dir"
)
