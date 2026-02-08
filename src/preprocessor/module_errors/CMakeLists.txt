# -----------------------------------------------------
# src/module_errors/CMakeLists.txt
# CMakeLists.txt for module_errors
#
# This module handles detection and reporting of
# preprocessing errors and warnings.
# It is compiled as a static library.
# -----------------------------------------------------

# Create the static library from the module_errors source file
add_library(module_errors module_errors.c)

# Include the current source directory for header file access
target_include_directories(module_errors PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Link against utils if needed by the preprocessor infrastructure
target_link_libraries(module_errors PRIVATE utils)

# Status message
message(STATUS "(${PROJECT_NAME}) Module_errors configured: Added as static library")
