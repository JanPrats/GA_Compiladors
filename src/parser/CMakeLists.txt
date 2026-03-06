# -----------------------------------------------------------------------------
# src/parser/CMakeLists.txt
# 
# This file configures the parser executable and its module libraries.
# The parser performs syntactic analysis on tokenized input using SRA.
#
# Structure:
# - Parser executable: links main.c, config.c with module libraries
# - Module libraries: module_load_tokens, module_parser
#
# The parser reads tokenized files (.cscn) and performs parsing.
# ----------------------------------------------------------------------------- 

message(STATUS "(${PROJECT_NAME}) Configuring parser module...")

# Add parser module subdirectories
add_subdirectory(load_tokens)
add_subdirectory(module_parser)

message(STATUS "   - (${PROJECT_NAME}) Added parser module subdirectories")

# Create the parser executable
add_executable(parser
    main.c
    config.c
)

# Link parser with module libraries
target_link_libraries(parser PRIVATE
    module_load_tokens
    module_parser_prs
)

# Include directories for parser
target_include_directories(parser PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

message(STATUS "(${PROJECT_NAME}) Parser executable configured: main parser binaries linked with modules")