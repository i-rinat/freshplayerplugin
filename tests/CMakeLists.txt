enable_testing()
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})

add_definitions(-DTEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")

set(test_list
    test_header_parser
    test_ppb_char_set
    test_ppb_flash_file
    test_ppb_url_request_info
    test_ppb_x509_certificate
    test_uri_parser
)

link_directories(
    ${REQ_LIBRARY_DIRS}
)

# simplify inclusion of .c sources
include_directories(..)

foreach(item ${test_list})
    add_executable(${item} EXCLUDE_FROM_ALL
        ${item}.c
        $<TARGET_OBJECTS:freshwrapper-obj>
        $<TARGET_OBJECTS:parson-obj>
        $<TARGET_OBJECTS:uri-parser-obj>
        ../src/config_pepperflash.c
        common.c)
    target_link_libraries(${item}
        "-Wl,-z,muldefs"
        ${REQ_LIBRARIES})
    add_test(${item} ${item})
    add_dependencies(check ${item})
endforeach()

add_executable(util_glx_pixmap EXCLUDE_FROM_ALL util_glx_pixmap.c)
add_dependencies(check util_glx_pixmap)
target_link_libraries(util_glx_pixmap ${REQ_LIBRARIES})
