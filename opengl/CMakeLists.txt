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

find_package(OpenGL REQUIRED)

add_executable(opengl)
target_sources(
    opengl
    PRIVATE
    main.cpp
)
set_target_properties(
    opengl
    PROPERTIES
    MACOSX_BUNDLE ON
)
target_compile_features(
    opengl
    PRIVATE
    cxx_std_17
)
if(APPLE)
    target_compile_definitions(
        opengl
        PRIVATE
        GL_SILENCE_DEPRECATION
    )
endif()
target_link_libraries(
    opengl
    PRIVATE
    glfw
    OpenGL::GL
)
