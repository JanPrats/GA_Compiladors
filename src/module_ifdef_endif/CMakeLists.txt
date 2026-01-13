# -----------------------------------------------------
# src/module_ifdef_endif/CMakeLists.txt
# CMakeLists.txt for module_ifdef_endif
#
# This module handles conditional compilation using
# ifdef / endif directives.
# It is compiled as a static library.
# -----------------------------------------------------

# Create the static library from the module_ifdef_endif source file
add_library(module_ifdef_endif module_ifdef_endif.c)

# Include the current source directory for header file access
target_include_directories(module_ifdef_endif PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Link against utils if needed by the preprocessor infrastructure
target_link_libraries(module_ifdef_endif PRIVATE utils)

# Status message
message(STATUS "(${PROJECT_NAME}) Module_ifdef_endif configured: Added as static library")
