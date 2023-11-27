add_executable(TinyRenderer)

set(modules
  file.ixx
)

target_sources(TinyRenderer PRIVATE
    ${modules}
)

if(BACKEND STREQUAL "dx12")
  target_sources(TinyRenderer PRIVATE
    main_dx12.cpp
  )
elseif(BACKEND STREQUAL "ogl")
  target_sources(TinyRenderer PRIVATE
    main_ogl.cpp
  )
endif()

target_include_directories(TinyRenderer PRIVATE
  ${PROJECT_SOURCE_DIR}/3rdParty/stb
)

target_link_libraries(TinyRenderer PRIVATE
  imgui
)
