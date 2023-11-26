set(imgui_source_dir
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui
)
set(imgui_backend_dir
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends
)

file(GLOB imgui_sources
    CONFIGURE_DEPENDS "${imgui_source_dir}/*.cpp"
)
file(GLOB imgui_impl_dx12
    CONFIGURE_DEPENDS "${imgui_backend_dir}/imgui_impl_dx12.cpp"
    CONFIGURE_DEPENDS "${imgui_backend_dir}/imgui_impl_win32.cpp"
)

add_library(imgui STATIC
    ${imgui_sources} ${imgui_impl_dx12}
)
target_include_directories(imgui PUBLIC
    $<BUILD_INTERFACE:${imgui_source_dir}>
    $<BUILD_INTERFACE:${imgui_backend_dir}>
)
target_link_libraries(imgui PUBLIC
    d3d12
    dxgi
)

set_property(TARGET imgui PROPERTY FOLDER "3rdParty"
)