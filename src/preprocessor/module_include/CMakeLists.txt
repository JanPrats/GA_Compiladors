# -----------------------------------------------------
# src/module_include/CMakeLists.txt
# CMakeLists.txt for module_include
#
# This module processes include directives by inserting
# referenced files into the source code.
# It is compiled as a static library.
# -----------------------------------------------------

# Create the static library from the module_include source file
add_library(module_include module_include.c)

# Include the current source directory for header file access
target_include_directories(module_include PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Link against utils if needed by the preprocessor infrastructure
target_link_libraries(module_include PRIVATE utils)

# Status message
message(STATUS "(${PROJECT_NAME}) Module_include configured: Added as static library")
