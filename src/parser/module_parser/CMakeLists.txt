message(STATUS "(${PROJECT_NAME}) Configuring module_parser_prs library...")

# Create static library for module_parser
add_library(module_parser_prs STATIC
    module_parser.c
)

# Include directories for this module
target_include_directories(module_parser_prs PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

message(STATUS "(${PROJECT_NAME}) module_parser_prs: Added as static library")
