add_executable(metal_ditty)
set_target_properties(
    metal_ditty
    PROPERTIES
    MACOSX_BUNDLE ON
    RESOURCE Main.storyboard
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_LIST_DIR}/MacOSXBundleInfo.plist.in
)
target_sources(
    metal_ditty
    PRIVATE
    AppController.swift
    Renderer.swift
    ViewController.swift
    Main.storyboard
)
