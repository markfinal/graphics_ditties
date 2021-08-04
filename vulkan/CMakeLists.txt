include(FetchContent)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.3.4
    GIT_PROGRESS   TRUE
    USES_TERMINAL_DOWNLOAD TRUE
)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
    vulkan_headers
    GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers.git
    GIT_TAG        sdk-1.2.182.0
    GIT_PROGRESS   TRUE
    USES_TERMINAL_DOWNLOAD TRUE
)
FetchContent_MakeAvailable(vulkan_headers)

FetchContent_Declare(
    vulkan_loader
    GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Loader.git
    GIT_TAG        sdk-1.2.182.0
    GIT_PROGRESS   TRUE
    USES_TERMINAL_DOWNLOAD TRUE
)
FetchContent_MakeAvailable(vulkan_loader)

add_executable(vulkan_ditty)
target_sources(
    vulkan_ditty
    PRIVATE
    main.cpp
)
target_compile_features(
    vulkan_ditty
    PRIVATE
    cxx_std_17
)
set_target_properties(
    vulkan_ditty
    PROPERTIES
    MACOSX_BUNDLE ON
)
target_link_libraries(
    vulkan_ditty
    PRIVATE
    glfw
    Vulkan-Headers
    vulkan
)
