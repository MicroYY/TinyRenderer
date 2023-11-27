set(imgui_source_dir
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui
)
set(imgui_backend_dir
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends
)

file(GLOB imgui_sources
    CONFIGURE_DEPENDS "${imgui_source_dir}/*.cpp"
)

add_library(imgui STATIC)

if(BACKEND STREQUAL "dx12")
    file(GLOB imgui_backend
        CONFIGURE_DEPENDS "${imgui_backend_dir}/imgui_impl_dx12.cpp"
        CONFIGURE_DEPENDS "${imgui_backend_dir}/imgui_impl_win32.cpp"
    )
    target_link_libraries(imgui PUBLIC
        d3d12
        dxgi
    )
elseif(BACKEND STREQUAL "ogl")
    file(GLOB imgui_backend
        CONFIGURE_DEPENDS "${imgui_backend_dir}/imgui_impl_glfw.cpp"
        CONFIGURE_DEPENDS "${imgui_backend_dir}/imgui_impl_opengl3.cpp"
    )
    target_include_directories(imgui PUBLIC
        ${imgui_source_dir}/examples/libs/glfw/include
    )
    target_link_libraries(imgui PUBLIC
        opengl32
        ${imgui_source_dir}/examples/libs/glfw/lib-vc2010-64/glfw3.lib
    )
endif()

target_sources(imgui PRIVATE
    ${imgui_sources} ${imgui_backend}
)

target_include_directories(imgui PUBLIC
    $<BUILD_INTERFACE:${imgui_source_dir}>
    $<BUILD_INTERFACE:${imgui_backend_dir}>
)

set_property(TARGET imgui PROPERTY FOLDER "3rdParty"
)