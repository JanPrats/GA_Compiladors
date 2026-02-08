/*
 * -----------------------------------------------------------------------------
 * module_ifdef_endif.h
 *
 * Header file for the ifdef/endif module, which provides functions to handle
 * conditional compilation directives in a modular and testable way.
 *
 * Functions:
 * - `process_ifdef`: Processes a #ifdef or #ifndef directive, including or excluding
 *                    code based on macro definitions.
 *
 * Usage:
 *     Include this header in parser modules to access conditional compilation
 *     functionality.
 *
 * Notes:
 *     This is part of a modular project structure, allowing each module to be
 *     developed and tested independently.
 * 
 * Team: GA
 * Contributor/s: Marc Rodríguez Vitolo
 * -----------------------------------------------------------------------------
 */


#ifndef MODULE_IFDEF_ENDIF_H
#define MODULE_IFDEF_ENDIF_H

#include "../main.h"
#include <stdbool.h>

/*
 * The ifdef/endif module manages conditional compilation
 * by including or excluding code based on macro definitions.
 */

// Forward declaration of ParserState structure
typedef struct ParserState ParserState;

// Process #ifdef or #ifndef directive
int process_ifdef(ParserState* state, bool is_ifndef, bool copy_to_output);


#endif
