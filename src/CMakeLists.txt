add_executable(TinyRenderer)

set(modules
  file.ixx
)

target_sources(TinyRenderer
  PRIVATE
    ${modules}
    main.cpp
)

target_include_directories(TinyRenderer PRIVATE
  ${PROJECT_SOURCE_DIR}/3rdParty/stb
)

target_link_libraries(TinyRenderer PRIVATE
  imgui
)
