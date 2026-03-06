message(STATUS "(${PROJECT_NAME}) Configuring module_load_tokens library...")

# Create static library for module_load_tokens
add_library(module_load_tokens STATIC
    module_load_tokens.c
)

# Include directories for this module
target_include_directories(module_load_tokens PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

message(STATUS "(${PROJECT_NAME}) module_load_tokens: Added as static library")
