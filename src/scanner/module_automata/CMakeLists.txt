# -----------------------------------------------------
# src/scanner/module_automata/CMakeLists.txt
# CMakeLists.txt for module_automata
#
# This module implements the DFA (Deterministic Finite Automata) engines
# for lexical analysis. It includes:
# - Individual DFA automata for each token category
# - Automata driver that processes input and generates tokens
# - Token classification and output
#
# It is compiled as a static library and linked into the scanner executable.
# Note: define_automata.c is a helper for automata creation.
# Others include: module_automata.c, and multiple category-specific automata files.
# -----------------------------------------------------

# Create the static library from the module_automata source files
add_library(module_automata STATIC
    module_automata.c
    define_automata.c
)

# Include the current source directory for header file access
target_include_directories(module_automata PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Print a status message during CMake configuration
message(STATUS "(${PROJECT_NAME}) module_automata configured: Added as static library")

# Optionally, you can set properties or compile options specific to this module