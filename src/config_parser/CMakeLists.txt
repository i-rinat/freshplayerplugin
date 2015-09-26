set(CONFIG_PARSER_FSM_C "${CMAKE_CURRENT_BINARY_DIR}/config_parser_fsm.c")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
add_custom_command(
    OUTPUT "${CONFIG_PARSER_FSM_C}"
    DEPENDS config_parser_fsm.rl.c
    COMMAND "${RAGEL}"
    ARGS    -G2 "${CMAKE_CURRENT_SOURCE_DIR}/config_parser_fsm.rl.c" -o "${CONFIG_PARSER_FSM_C}"
    VERBATIM
)

add_custom_target(generate-config-parser-fsm DEPENDS "${CONFIG_PARSER_FSM_C}")
add_library(config-parser-obj OBJECT
    config_parser.c
)

add_dependencies(config-parser-obj generate-config-parser-fsm)
