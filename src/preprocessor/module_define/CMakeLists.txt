# -----------------------------------------------------
# src/module_define/CMakeLists.txt
# CMakeLists.txt for module_define
#
# This module handles macro definitions and replacement.
# It is compiled as a static library.
# -----------------------------------------------------

# Create the static library from the module_define source file
add_library(module_define module_define.c)

# Include the current source directory for header file access
target_include_directories(module_define PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Link against utils if needed by the preprocessor infrastructure
target_link_libraries(module_define PRIVATE utils)

# Status message
message(STATUS "(${PROJECT_NAME}) Module_define configured: Added as static library")
