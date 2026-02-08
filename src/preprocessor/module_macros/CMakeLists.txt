# -----------------------------------------------------
# src/module_macros/CMakeLists.txt
# CMakeLists.txt for module_macros
#
# This module handles macro expansion during preprocessing.
# It is compiled as a static library.
# -----------------------------------------------------

# Create the static library from the module_macros source file
add_library(module_macros module_macros.c)

# Include the current source directory for header file access
target_include_directories(module_macros PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Link against utils if needed by the preprocessor infrastructure
target_link_libraries(module_macros PRIVATE utils)

# Status message
message(STATUS "(${PROJECT_NAME}) Module_macros configured: Added as static library")
