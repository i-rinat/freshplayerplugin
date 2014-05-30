enable_testing()
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})

set(test_list
    test_header_parser
    test_ppb_flash_file_modulelocal
)

link_directories(${REQ_LIBRARY_DIRS})

# simplify inclusion of .c sources
include_directories(..)


foreach(item ${test_list})
    add_executable(${item} EXCLUDE_FROM_ALL ${item}.c ../src/trace.c)
    target_link_libraries(${item} -ldl ${REQ_LIBRARIES})
    add_test(${item} ${item})
    add_dependencies(check ${item})
endforeach()
