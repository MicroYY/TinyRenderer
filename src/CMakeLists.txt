add_executable(TinyRenderer)

set(modules
  io.ixx
  math.ixx
  model.ixx
  scene.ixx
  shader.ixx
  tgaimage.ixx
  tiny_render.ixx
  geometry.ixx
)

target_sources(TinyRenderer PRIVATE
    ${modules}
)

target_link_libraries(TinyRenderer PRIVATE
  imgui
  imguiFileDialog
)

if(BACKEND STREQUAL "dx12")
  target_sources(TinyRenderer PRIVATE
    main_dx12.cpp
  )
else()
  target_sources(TinyRenderer PRIVATE
    main_ogl.cpp
  )
  target_link_libraries(TinyRenderer PRIVATE
    glad
)
endif()

target_include_directories(TinyRenderer PRIVATE
  ${PROJECT_SOURCE_DIR}/3rdParty/stb
)

option(ENABLE_GRAPHICSAPI_DEMO "enable graphics API demo" ON)

if(ENABLE_GRAPHICSAPI_DEMO)
  include(graphicsAPIdemo/graphicsAPIdemo.cmake)
endif()