add_executable(d3d12)
target_sources(
    d3d12
    PRIVATE
    main.cpp
)

target_link_libraries(
    d3d12
    PRIVATE
    d3d12.lib
    dxgi.lib
    dxguid.lib
)
