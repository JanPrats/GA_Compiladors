/*
 * -----------------------------------------------------------------------------
 * module_define.h
 *
 * Header file for the define module, which provides functions to handle macro
 * definitions in a modular and testable way.
 *
 * Functions:
 * - `process_define`: Processes a #define directive, adding or updating macros
 *                     in the macro dictionary.
 * - `substitute_macro`: Checks if an identifier is a defined macro and returns
 *                       its value if it exists.
 * - `is_macro_defined`: Checks whether a macro with a given name is already defined.
 *
 * Usage:
 *     Include this header in parser modules or test modules that require access
 *     to macro handling functionality.
 * 
 * Notes:
 *     This is part of a modular project structure, allowing each module to be
 *     developed and tested independently.
 * 
 * Team: GA
 * Contributor/s: Clara Serra Borràs
 * -----------------------------------------------------------------------------
 */

#ifndef MODULE_DEFINE_H
#define MODULE_DEFINE_H

#include "../main.h"
#include <stdbool.h>

//estructures externes
typedef struct ParserState ParserState;
typedef struct MacroDict MacroDict;

// Process #define directive
int process_define(ParserState* state);

// Check if identifier is a macro and substitute
char* substitute_macro(ParserState* state, const char* identifier);

// Check if macro is defined
bool is_macro_defined(MacroDict* dict, const char* name);

#endif // MODULE_DEFINE_H
