/*
 * -----------------------------------------------------------------------------
 * module_ifdef_endif.c
 *
 * This module provides functionality to handle conditional compilation directives.
 *
 * - `process_ifdef`: Processes a #ifdef or #ifndef directive, including or excluding
 *                     code based on macro definitions.
 *
 * Usage:
 *     Called from the parser when processing lines containing #ifdef or #ifndef directives. 
 *
 * Status:
 *     Added error handling for missing #endif and unexpected #endif. Can handle #else blocks.
 *
 * Team: GA
 * Author: Marc Rodríguez Vitolo
 * -----------------------------------------------------------------------------
 */

#include "module_ifdef_endif.h"
#include "../module_parser/module_parser.h"
#include "../module_define/module_define.h"
#include "../module_errors/module_errors.h"
#include <string.h>

// Forward declaration
int parse_until(ParserState* state, const char** stop_symbols, bool copy_to_output);

// Process #ifdef or #ifndef directive
int process_ifdef(ParserState* state, bool is_ifndef, bool copy_to_output) {
    // Skip whitespace after the directive
    skip_whitespace(state);
    
    // Read macro name
    char* macro_name = read_word(state);
    if (!macro_name) {
        report_error(ERROR_ERROR, state->current_filename, state->current_line,
                   "#ifdef/#ifndef without macro name");
        return -1;
    }
    
    // Skip rest of line (consume the directive line)
    read_line(state);
    
    // Check if macro is defined
    bool is_defined = is_macro_defined(state->macro_dict, macro_name);
    
    // Determine if we should copy the if block
    bool should_copy_if_block;
    if (is_ifndef) {
        // #ifndef: copy if NOT defined
        should_copy_if_block = !is_defined && copy_to_output;
    } else {
        // #ifdef: copy if defined
        should_copy_if_block = is_defined && copy_to_output;
    }
    
    // Parse the if block until we hit #else or #endif
    // We provide both as stop symbols so we can detect which one we stopped at
    const char* stop_symbols[] = {"else", "endif", NULL};
    int result = parse_until(state, stop_symbols, should_copy_if_block);
    
    // Check what directive we stopped at
    if (result == 0) {
        // Stopped at #else - now parse the else block until #endif
        bool should_copy_else_block = !should_copy_if_block && copy_to_output;
        const char* endif_only[] = {"endif", NULL};
        parse_until(state, endif_only, should_copy_else_block);
        // parse_until consumed the #endif, we're done
    }
    else if (result == 1) {
        // Stopped at #endif directly (no #else block) - we're done
        // The #endif was already consumed by parse_until
    }
    else if (result == -1) {
        // Reached EOF without finding #endif - error
        report_error(ERROR_ERROR, state->current_filename, state->current_line,
                   "#ifdef without matching #endif (reached end of file)");
        return -1;
    }
    
    return 0;
}



