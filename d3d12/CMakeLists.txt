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
    DXHeaders
    GIT_REPOSITORY https://github.com/microsoft/DirectX-Headers.git
    GIT_TAG        v1.4.9
    GIT_PROGRESS   TRUE
    USES_TERMINAL_DOWNLOAD TRUE
)
FetchContent_MakeAvailable(DXHeaders)

add_executable(d3d12)
target_sources(
    d3d12
    PRIVATE
    main.cpp
)
target_compile_features(
    d3d12
    PRIVATE
    cxx_std_17
)
target_link_libraries(
    d3d12
    PRIVATE
    d3d12.lib
    dxgi.lib
    dxguid.lib
    glfw
    DirectX-Headers
)
