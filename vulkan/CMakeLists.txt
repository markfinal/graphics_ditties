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
if(APPLE)
    include(ExternalProject)

    # perform the fetchDependencies in the download, as if it runs in the build command
    # the whole set of projects rebuild
    ExternalProject_Add(
        moltenvk_ext
        DOWNLOAD_COMMAND git init
            COMMAND git remote add origin https://www.github.com/KhronosGroup/MoltenVK.git
            COMMAND git pull --depth=1 origin v1.1.4
            COMMAND <DOWNLOAD_DIR>/fetchDependencies --macos -v
        USES_TERMINAL_DOWNLOAD TRUE
        CONFIGURE_COMMAND ""
        BUILD_COMMAND make -C <DOWNLOAD_DIR> macos
        INSTALL_COMMAND ""
        SOURCE_DIR ""
        BUILD_IN_SOURCE TRUE
    )

    # this is a runtime dependency only
    add_dependencies(vulkan_ditty moltenvk_ext)

    ExternalProject_Get_property(moltenvk_ext DOWNLOAD_DIR)

    set(ContentsDir $<TARGET_BUNDLE_CONTENT_DIR:vulkan_ditty>)
    add_custom_command(
        TARGET vulkan_ditty
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${ContentsDir}/Resources/vulkan/icd.d/
        COMMAND ${CMAKE_COMMAND} -E copy ${DOWNLOAD_DIR}/Package/Release/MoltenVK/dylib/macOS/libMoltenVK.dylib ${ContentsDir}/Resources/vulkan/icd.d/
        COMMAND ${CMAKE_COMMAND} -E copy ${DOWNLOAD_DIR}/MoltenVK/icd/MoltenVK_icd.json ${ContentsDir}/Resources/vulkan/icd.d/
    )
endif()
