/*
 * -----------------------------------------------------------------------------
 * module_define.h
 *
 * This module provides functionality to handle macros in the parser.
 *
 * - `is_macro_defined`: Checks if a macro with a given name is already defined.
 * - `process_define`: Processes a #define directive, adding or updating macros
 *                     in the macro dictionary.
 * - `substitute_macro`: Returns the value of a macro if it exists, otherwise NULL.
 *
 * Usage:
 *     Called from the parser when processing lines containing #define directives
 *     or when substituting macro identifiers in the source code. Module also used 
 *     by the #ifdef module to check whether directives exist 
 *
 * Status:
 *     Implemented version, should handle leading/trailing whitespace and duplicate
 *     macro definitions. Maximum macros limit enforced.
 *
 * Team: GA
 * Author: Clara Serra Borràs
 * -----------------------------------------------------------------------------
 */

#include "module_define.h"
#include "../module_parser/module_parser.h"
#include "../module_errors/module_errors.h"
#include <string.h>
#include <ctype.h>

// Check if macro is defined
bool is_macro_defined(MacroDict* dict, const char* name) {
    for (int i = 0; i < dict->count; i++) { // all entries in the dictionary
        if (dict->entries[i].is_defined && strcmp(dict->entries[i].name, name) == 0) {   // Check if the current entry is defined and its name matches the given name
            return true;
        }
    }
    return false;
}

// Process #define directive
int process_define(ParserState* state) {
    // Skip whitespace
    skip_whitespace(state);

    // Read macro name
    char* macro_name = read_word(state);
    
    if (!macro_name) {
        report_error(ERROR_WARNING, state->current_filename, state->current_line,
                   "#define without macro name");
        return -1;
    }
    
    // Skip whitespace before value
    skip_whitespace(state);
    
    
    // Read macro value (rest of line)
    char* value = read_line(state);
    
    // Trim leading whitespace from the macro value
    // After this loop, value points to the first non-whitespace character
    while (*value && is_whitespace(*value)){
        value++;
    } 
    // Trim trailing whitespace from the macro value
    // After this loop, all trailing whitespace is removed and value points to a trimmed string
    char* end = value + strlen(value) - 1;
    while (end > value && is_whitespace(*end)) {
        *end = '\0';
        end--;
    }
        
    int index = -1;
    // Check if macro already exists   
    if (is_macro_defined(state->macro_dict, macro_name)) {
        for (int i = 0; i < state->macro_dict->count; i++) {
            if (strcmp(state->macro_dict->entries[i].name, macro_name) == 0) {
                index = i;
                break;
            }
        }
    }

    // Add or update macro
    if (index >= 0) { //ja existeix
        // Update existing macro
        strncpy(state->macro_dict->entries[index].value, value, MAX_MACRO_VALUE - 1);
        state->macro_dict->entries[index].value[MAX_MACRO_VALUE - 1] = '\0'; //ensure last value is null-terminated
    } else {
        
        if (state->macro_dict->count >= MAX_MACROS) {
            report_error(ERROR_WARNING, state->current_filename, state->current_line,
                       "Maximum number of macros reached, ignoring #define");
            return 0;
        } 
        // Add new macro      
        index = state->macro_dict->count++; //increment count after using current value
        strncpy(state->macro_dict->entries[index].name, macro_name, MAX_MACRO_NAME - 1); //copy name
        state->macro_dict->entries[index].name[MAX_MACRO_NAME - 1] = '\0'; //ensure last name is null-terminated
        strncpy(state->macro_dict->entries[index].value, value, MAX_MACRO_VALUE - 1); //copy value
        state->macro_dict->entries[index].value[MAX_MACRO_VALUE - 1] = '\0';
        state->macro_dict->entries[index].is_defined = true; // Mark as defined 
    }
    
    return 0;
}

// Substitute macro if it exists
char* substitute_macro(ParserState* state, const char* identifier) {
    for (int i = 0; i < state->macro_dict->count; i++) { // Loop through all macros in the dictionary
        if (state->macro_dict->entries[i].is_defined &&
            strcmp(state->macro_dict->entries[i].name, identifier) == 0) { //chcek if defined and name matches
            return state->macro_dict->entries[i].value; // Return the macro value
        }
    }
    return NULL; // Not a macro
}

