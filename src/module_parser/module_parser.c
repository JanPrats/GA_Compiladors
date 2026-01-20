// POL, CLARA, MARC, ANDREA, GORKA, JAN

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "./module_parser.h"
#include "../module_comments_remove/comments_remove.h"
#include "../module_include/module_include.h"
#include "../module_define/module_define.h"
#include "../module_comments_remove/module_comments_remove.h"
#include "../module_ifdef_endif/module_ifdef_endif.h"
#include "../module_errors/module_errors.h"


// Creates and initializes a new ParserState structure.
// This function prepares everything needed before starting the parsing process.
ParserState* init_parser(const char* input_file,
                         const char* output_file,
                         PreprocessorFlags* flags) 
{
    // Allocate memory for the main parser state
    ParserState* state = (ParserState*)malloc(sizeof(ParserState));

    // Allocate and initialize the macro dictionary
    // This will store all defined macros during preprocessing
    state->macro_dict = (MacroDict*)malloc(sizeof(MacroDict));
    state->macro_dict->count = 0;

    // Clear all macro entries to ensure a clean starting state
    memset(state->macro_dict->entries, 0, sizeof(state->macro_dict->entries));

    // Open the input file in read mode
    // This file will be parsed line by line
    state->current_file = fopen(input_file, "r");

    // Open the output file in write mode
    // The processed result will be written here
    state->output_file = fopen(output_file, "w");

    // Store the current filename safely (prevent buffer overflow)
    strncpy(state->current_filename, input_file, MAX_FILENAME - 1);
    state->current_filename[MAX_FILENAME - 1] = '\0';

    // Initialize line tracking (used for error reporting and debugging)
    state->current_line = 1;

    // Copy preprocessor behavior flags
    state->remove_comments     = flags->remove_comments;
    state->process_directives  = flags->process_directives;

    // Initial parser state flags
    state->in_string     = false;  // Not currently inside a string literal
    state->in_comment    = false;  // Not currently inside a comment
    state->has_lookahead = false;  // No buffered lookahead character

    // Return the fully initialized parser state
    return state;
}


// Releases all resources associated with the parser state.
// Safely closes files and frees allocated memory.
void cleanup_parser(ParserState* state) {
    if (state) {
        // Close the input file if it was opened
        if (state->current_file) fclose(state->current_file);

        // Close the output file if it was opened
        if (state->output_file) fclose(state->output_file);

        // Free the macro dictionary used by the preprocessor
        if (state->macro_dict) free(state->macro_dict);

        free(state);
    }
}

// Reads and consumes a single character from the input stream.
// Handles lookahead buffering and updates the current line counter.
char read_char(ParserState* state) {
    // If a lookahead character exists, consume it first
    if (state->has_lookahead) {
        state->has_lookahead = false;
        return state->lookahead;
    }
    
    // Read the next character from the input file
    int c = fgetc(state->current_file);

    // Track line numbers for error reporting and diagnostics
    if (c == '\n') {
        state->current_line++;
    }

    // Return null character on EOF, otherwise return the read character
    return (c == EOF) ? '\0' : (char)c;
}

// Returns the next character without consuming it.
// Uses a lookahead buffer to preserve the input stream state.
char peek_char(ParserState* state) {
    // If a lookahead character is already stored, return it
    if (state->has_lookahead) {
        return state->lookahead;
    }
    
    // Read the next character from the file without advancing the parser
    int c = fgetc(state->current_file);

    if (c != EOF) {
        // Store the character for future consumption
        state->lookahead = (char)c;
        state->has_lookahead = true;
        return state->lookahead;
    }

    // Return null character if end of file is reached
    return '\0';
}
