# -----------------------------------------------------
# src/scanner/module_error/CMakeLists.txt
# CMakeLists.txt for module_error
#
# This module handles error tracking and reporting during lexical analysis.
# It manages error initialization, counting, and finalization.
#
# It is compiled as a static library and linked into the scanner executable.
# -----------------------------------------------------

# Create the static library from the module_error source file
add_library(module_error STATIC module_error.c)

# Include the current source directory for header file access
target_include_directories(module_error PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Print a status message during CMake configuration
message(STATUS "(${PROJECT_NAME}) module_error configured: Added as static library")

# Optionally, you can set properties or compile options specific to this module