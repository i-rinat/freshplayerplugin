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

add_custom_target(img-resource-srcs DEPENDS "${CCBD}/img_resources.h" "${CCBD}/img_resources.c")
add_executable(stringify_images stringify_images.c)
add_library(img-resources STATIC "${CCBD}/img_resources.c")
add_dependencies(img-resources img-resource-srcs)
