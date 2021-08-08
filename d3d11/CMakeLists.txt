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

add_executable(d3d11_ditty)
target_sources(
    d3d11_ditty
    PRIVATE
    main.cpp
)
target_compile_features(
    d3d11_ditty
    PRIVATE
    cxx_std_17
)
target_link_libraries(
    d3d11_ditty
    PRIVATE
    glfw
    d3d11.lib
    #dxgi.lib
    #dxguid.lib
)
