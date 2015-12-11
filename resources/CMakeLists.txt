set(GENERATED_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)
set(CCBD "${CMAKE_CURRENT_BINARY_DIR}")

set(text_resource_list
    "${CMAKE_CURRENT_SOURCE_DIR}/libpdf_frontend.js"
)

add_custom_command(
    OUTPUT "${CCBD}/txt_resources.h" "${CCBD}/txt_resources.c"
    COMMAND "${CCBD}/stringify_text" "${CCBD}/txt_resources.h" "${CCBD}/txt_resources.c"
            ${text_resource_list}
    DEPENDS ${text_resource_list} stringify_text
)

add_custom_target(txt-resource-srcs DEPENDS "${CCBD}/txt_resources.h" "${CCBD}/txt_resources.c")

add_executable(stringify_text stringify_text.c)

add_library(txt-resources STATIC "${CCBD}/txt_resources.c")
add_dependencies(txt-resources txt-resource-srcs)
