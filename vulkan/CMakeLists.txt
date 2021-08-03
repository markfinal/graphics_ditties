include(FetchContent)
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.3.4
)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
FetchContent_MakeAvailable(glfw)

add_executable(vulkan)
target_sources(
    vulkan
    PRIVATE
    main.cpp
)
set_target_properties(
    vulkan
    PROPERTIES
    MACOSX_BUNDLE ON
)
target_link_libraries(
    vulkan
    PRIVATE
    glfw
)
