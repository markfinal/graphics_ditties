add_executable(metal)
set_target_properties(
    metal
    PROPERTIES
    MACOSX_BUNDLE ON
    RESOURCE Main.storyboard
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_LIST_DIR}/MacOSXBundleInfo.plist.in
)
target_sources(
    metal
    PRIVATE
    AppController.swift
    Renderer.swift
    ViewController.swift
    Main.storyboard
)
