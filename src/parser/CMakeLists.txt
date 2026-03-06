# -----------------------------------------------------------------------------
# src/parser/CMakeLists.txt
#
# Configures the parser executable.
# The parser reads .cscn token files and runs an SRA (Shift-Reduce Automaton).
# -----------------------------------------------------------------------------

message(STATUS "(${PROJECT_NAME}) Configuring parser module...")

# Create the parser executable
add_executable(parser
    main.c
    config.c
    load_tokens/module_load_tokens.c
    module_parser/module_parser.c
)

# Include directories for parser
target_include_directories(parser PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

message(STATUS "(${PROJECT_NAME}) Parser executable configured")
