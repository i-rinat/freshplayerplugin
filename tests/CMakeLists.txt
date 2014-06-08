enable_testing()
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})

set(test_list
    test_header_parser
    test_ppb_flash_file_modulelocal
    test_ppb_url_request_info
)

link_directories(${REQ_LIBRARY_DIRS})

# simplify inclusion of .c sources
include_directories(..)


foreach(item ${test_list})
    add_executable(${item} EXCLUDE_FROM_ALL ${item}.c ../src/trace.c)
    target_link_libraries(${item} -ldl ${REQ_LIBRARIES} freshwrapper-static)
    add_test(${item} ${item})
    add_dependencies(check ${item})
endforeach()


# ensure libfreshwrapper is linked with all libraries it uses
add_executable(dep-check dep-check.c)
link_directories(${CMAKE_BINARY_DIR})
target_link_libraries(dep-check freshwrapper)
