set(GENERATED_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)
file(GLOB pictogram_list ${CMAKE_SOURCE_DIR}/3rdparty/chromium/default_100_percent/common/pdf/*.png)
string(REPLACE ";" " " pictograms "${pictogram_list}")

set(CCBD "${CMAKE_CURRENT_BINARY_DIR}")

add_custom_command(
    OUTPUT "${CCBD}/img_resources.h" "${CCBD}/img_resources.c"
    COMMAND "${CCBD}/img_resource_compiler" "${CCBD}/img_resources.h" "${CCBD}/img_resources.c"
            ${pictogram_list}
    DEPENDS ${pictogram_list} img_resource_compiler
)

add_custom_target(img-resource-srcs DEPENDS "${CCBD}/img_resources.h" "${CCBD}/img_resources.c")
add_executable(img_resource_compiler img_resource_compiler.c)
add_library(img-resources STATIC "${CCBD}/img_resources.c")
add_dependencies(img-resources img-resource-srcs)
