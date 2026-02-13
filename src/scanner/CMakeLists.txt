# -----------------------------------------------------------------------------
# src/scanner/CMakeLists.txt
# 
# This file configures the scanner executable and its module libraries.
# The scanner performs lexical analysis on C source files using DFA automata.
#
# Structure:
# - Scanner executable: links main.c, config.c, count.c with module libraries
# - Module libraries: module_init, module_error, module_automata
#
# The scanner reads C source files and produces tokenized output.
# Each token is classified by category (number, identifier, keyword, etc.)
# using finite automata DFAs.
# -----------------------------------------------------------------------------

message(STATUS "(${PROJECT_NAME}) Configuring scanner module...")

# Add scanner module subdirectories
add_subdirectory(module_init)
add_subdirectory(module_error)
add_subdirectory(module_automata)

message(STATUS "   - (${PROJECT_NAME}) Added scanner module subdirectories")

# Create the scanner executable
add_executable(scanner
    main.c
    config.c
    count.c
)

# Link scanner with module libraries
target_link_libraries(scanner PRIVATE
    module_init
    module_error
    module_automata
)

# Include directories for scanner
target_include_directories(scanner PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

message(STATUS "(${PROJECT_NAME}) Scanner executable configured: main scanner binaries linked with modules")