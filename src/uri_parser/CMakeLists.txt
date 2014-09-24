set(URI_PARSER_FSM_C "${CMAKE_CURRENT_BINARY_DIR}/uri_parser_fsm.c")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/uri_parser_fsm.c"
    DEPENDS uri_parser_fsm.rl
    COMMAND "${RAGEL}"
    ARGS    -G2 "${CMAKE_CURRENT_SOURCE_DIR}/uri_parser_fsm.rl" -o "${URI_PARSER_FSM_C}"
    VERBATIM
)
add_custom_target(generate_fsm DEPENDS "${URI_PARSER_FSM_C}")
add_library(uri-parser-obj OBJECT
    uri_parser.c
)
add_dependencies(uri-parser-obj generate_fsm)
