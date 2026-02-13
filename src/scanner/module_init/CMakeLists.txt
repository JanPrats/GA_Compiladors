# -----------------------------------------------------
# src/scanner/module_init/CMakeLists.txt
# CMakeLists.txt for module_init
#
# This module initializes the scanner application:
# - Processes command-line arguments
# - Sets up automata DFAs for token recognition
# - Initializes status structure and file handles
#
# It is compiled as a static library and linked into the scanner executable.
# -----------------------------------------------------

# Create the static library from the module_init source file
add_library(module_init STATIC module_init.c)

# Include the current source directory for header file access
target_include_directories(module_init PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Print a status message during CMake configuration
message(STATUS "(${PROJECT_NAME}) module_init configured: Added as static library")

# Optionally, you can set properties or compile options specific to this module