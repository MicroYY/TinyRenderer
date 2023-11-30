add_executable(ogl_demo)

set(modules
  ${CMAKE_CURRENT_SOURCE_DIR}/shader.ixx
)

target_sources(ogl_demo PRIVATE
    ${modules}
)

target_sources(ogl_demo PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/test_ogl.cpp
)
target_link_libraries(ogl_demo PRIVATE
    glad
    imgui
)

target_include_directories(ogl_demo PRIVATE
  ${PROJECT_SOURCE_DIR}/3rdParty/stb
)

set_property(TARGET ogl_demo PROPERTY FOLDER "graphics API demo"
)