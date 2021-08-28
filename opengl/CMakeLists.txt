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

find_package(OpenGL REQUIRED)

add_executable(opengl_ditty)
target_sources(
    opengl_ditty
    PRIVATE
    main.cpp
)
set_target_properties(
    opengl_ditty
    PROPERTIES
    MACOSX_BUNDLE ON
)
target_compile_features(
    opengl_ditty
    PRIVATE
    cxx_std_17
)
if(APPLE)
    target_compile_definitions(
        opengl_ditty
        PRIVATE
        GL_SILENCE_DEPRECATION
    )
endif()
target_link_libraries(
    opengl_ditty
    PRIVATE
    glfw
    OpenGL::GL
)
