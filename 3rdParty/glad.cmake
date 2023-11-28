set(glad_source_dir
    ${CMAKE_CURRENT_SOURCE_DIR}/glad
)

file(GLOB glad
    CONFIGURE_DEPENDS "${glad_source_dir}/src/glad.c"
)

add_library(glad STATIC
    ${glad}
)
target_include_directories(glad PUBLIC
    $<BUILD_INTERFACE:${glad_source_dir}/include>
)
set_target_properties(glad PROPERTIES LINKER_LANGUAGE C)

set_property(TARGET glad PROPERTY FOLDER "3rdParty"
)