set(stb_source_dir
    ${CMAKE_CURRENT_SOURCE_DIR}/stb
)

file(GLOB stb
    CONFIGURE_DEPENDS "${stb_source_dir}/*.h"
)

add_library(stb STATIC
    ${stb}
)
target_include_directories(stb PUBLIC
    $<BUILD_INTERFACE:${stb_source_dir}>
)
set_target_properties(stb PROPERTIES LINKER_LANGUAGE C)

set_property(TARGET stb PROPERTY FOLDER "3rdParty"
)