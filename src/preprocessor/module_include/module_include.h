/*
 * -----------------------------------------------------------------------------
 * module_include.h
 *
 * Header file for the include module, which provides functions to handle
 * #include directives in a modular and testable way.
 *
 * Functions:
 * - `process_include`: Processes a #include directive, extracting the filename
 *                      and inserting the content of the referenced file into
 *                      the output stream.
 * - `module_include_run`: Test function that prints module loading confirmation.
 *
 * Usage:
 *     Called by the parser when it detects an #include directive during
 *     preprocessing. Supports both local includes ("file.h") and system
 *     includes (<file.h>).
 * 
 * Notes:
 *     This is part of a modular project structure, allowing each module to be
 *     developed and tested independently. The module prevents infinite recursion
 *     by tracking include depth with a maximum limit of 64 levels.
 * 
 * Team: GA
 * Contributor/s: Gorka Hernández Villalón
 * -----------------------------------------------------------------------------
 */

#ifndef MODULE_INCLUDE_H
#define MODULE_INCLUDE_H

#include "../main.h"
#include <stdbool.h>

typedef struct ParserState ParserState;

int process_include(ParserState* state, bool copy_to_output);

void module_include_run(void);

#endif
