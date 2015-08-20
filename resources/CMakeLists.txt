set(GENERATED_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)
file(GLOB pictogram_list ${CMAKE_SOURCE_DIR}/3rdparty/chromium/default_100_percent/common/pdf/*.png)
string(REPLACE ";" " " pictograms "${pictogram_list}")

set(CCBD "${CMAKE_CURRENT_BINARY_DIR}")

add_custom_command(
    OUTPUT "${CCBD}/img_resources.h" "${CCBD}/img_resources.c"
    COMMAND "${CCBD}/stringify_images" "${CCBD}/img_resources.h" "${CCBD}/img_resources.c"
            ${pictogram_list}
    DEPENDS ${pictogram_list} stringify_images
)

set(text_resource_list
    "${CMAKE_CURRENT_SOURCE_DIR}/libpdf_frontend.js"
)

add_custom_command(
    OUTPUT "${CCBD}/txt_resources.h" "${CCBD}/txt_resources.c"
    COMMAND "${CCBD}/stringify_text" "${CCBD}/txt_resources.h" "${CCBD}/txt_resources.c"
            ${text_resource_list}
    DEPENDS ${text_resource_list} stringify_text
)

add_custom_target(img-resource-srcs DEPENDS "${CCBD}/img_resources.h" "${CCBD}/img_resources.c")
add_custom_target(txt-resource-srcs DEPENDS "${CCBD}/txt_resources.h" "${CCBD}/txt_resources.c")

add_executable(stringify_images stringify_images.c)
add_executable(stringify_text stringify_text.c)

add_library(img-resources STATIC "${CCBD}/img_resources.c")
add_dependencies(img-resources img-resource-srcs)

add_library(txt-resources STATIC "${CCBD}/txt_resources.c")
add_dependencies(txt-resources txt-resource-srcs)
