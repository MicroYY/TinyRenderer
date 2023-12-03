set(imguiFD_source_dir
    ${CMAKE_CURRENT_SOURCE_DIR}/ImGuiFileDialog
)

file(GLOB imguiFD_src
    CONFIGURE_DEPENDS "${imguiFD_source_dir}/*.cpp"
)

add_library(imguiFileDialog STATIC
    ${imguiFD_src}
)
target_include_directories(imguiFileDialog PUBLIC
    $<BUILD_INTERFACE:${imguiFD_source_dir}>
)
target_link_libraries(imguiFileDialog PRIVATE
    imgui
)

set_property(TARGET imguiFileDialog PROPERTY FOLDER "3rdParty"
)