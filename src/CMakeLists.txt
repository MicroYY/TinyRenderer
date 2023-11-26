add_executable(TinyRenderer)

target_sources(TinyRenderer
  PRIVATE
    main.cpp
)

target_link_libraries(TinyRenderer PRIVATE imgui)