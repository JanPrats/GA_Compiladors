/*
 * -----------------------------------------------------------------------------
 * module_parser.c
 *
 * Core Parsing Engine.
 *
 * This module implements the main logic of the preprocessor. It reads the input source file character by character, detects tokens, and dispatches actions to
 * other specific modules (define, include, ifdef, etc.).
 *
 * Responsibilities:
 * - Initialize and clean up the ParserState and MacroDictionary.
 * - Provide low-level stream handling (read, peek, unread) with lookahead.
 * - Implement the main recursive parsing loop (`parse_until`).
 * - Identify and dispatch preprocessor directives (#include, #define, etc.).
 * - Handle macro substitution and comment removal.
 * - Track line numbers and context (strings, comments) for error reporting.
 *
 * Main functions:
 * - init_parser(): Allocates memory and sets initial state.
 * - parse_until(): The main loop that processes text until a stop symbol (EOF or else) is found.
 * - read_char() / peek_char(): Handles input stream buffering.
 * - read_word(): Extracts identifiers for macro checking.
 *
 * Design notes:
 * - Uses a single-character lookahead buffer (`state->lookahead`) to make parsing decisions without consuming tokens prematurely.
 * - `parse_until` is designed to be recursive or iterative depending on usage, allowing it to handle nested blocks (like nested #ifdefs) if needed.
 * - The module acts as the "Controller", delegating specific directive logic to helper modules while maintaining the global state.
 *
 * Authors: Pol, Clara, Marc, Andrea, Gorka, Jan
 * -----------------------------------------------------------------------------
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "module_parser.h"
#include "../module_comments_remove/module_comments_remove.h"
#include "../module_include/module_include.h"
#include "../module_define/module_define.h"
#include "../module_comments_remove/module_comments_remove.h"
#include "../module_ifdef_endif/module_ifdef_endif.h"
#include "../module_errors/module_errors.h"


// Creates and initializes a new ParserState structure.
// This function prepares everything needed before starting the parsing process.
ParserState* init_parser(const char* input_file,
                         const char* output_file,
                         ArgFlags* flags) 
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

// Puts one character back into the parser stream. We only allow ONE character of lookahead.
//WHEN TO USE: when the parser reads one character too far and needs to undo that read so another function can handle it.
void unread_char(ParserState* state, char c) {
    state->lookahead = c;
    state->has_lookahead = true;

    /* If we put back a newline, we must also restore the line counter */
    if (c == '\n') {
        state->current_line--;
    }
}

//Checks if a character is considered whitespace (are used to separate tokens, are ignored by the parser)
bool is_whitespace(char c) {
    return (
        c == ' '  ||   /* space */
        c == '\t' ||   /* tab */
        c == '\n' ||   /* newline */
        c == '\r'      /* carriage return */
    );
}

//Checks if the character can be part of an identifier (identifier may contain: letter,digits or underscode)
bool is_identifier_char(char c) {
    return isalnum(c) || c == '_';
}

//Skips all whitespace in the input stream.
//Moves the parser forward until a non-whitespace character is found.
void skip_whitespace(ParserState* state) {
    char c;

    while ((c = peek_char(state)) && is_whitespace(c)) {
        read_char(state);
    }
}


// Read a whole word (identifier or keyword), not just 1 char. By Identifier we mean macro names, directive names and variable-like tokens that might be macros
// We will use this when we are not in a comment or in a string to look ahead the whole word to know if we need to subtitute it, not just each character of the word individually.
char* read_word(ParserState* state) {
    
    static char word[MAX_MACRO_NAME]; //static so that it is not only local to the function and can be returned
    int i = 0; //count the number of characters in the word
    char c = peek_char(state); //See next character without actually reading it

    if (!isalpha(c) && c != '_') { //It is not a word we want to read completely.
        return NULL;
    }
    
    while ((c = peek_char(state)) && is_identifier_char(c) && i < MAX_MACRO_NAME - 1) { //If we still have buffer to write on and it is an identifier character
        word[i] = read_char(state); //add the character to the word
        i++;
    }
    word[i] = '\0';
    
    if (i > 0) { //If we actually have at least 1 character
        return word;
    }
    return NULL; //else return NULL
}

// Read until the end of the line
// In theory used when we want to read a whole line without caring about what there is inside of it. In case we need it.
// Note this also works when calling it mid-line, so it reads the rest of the line at once
char* read_line(ParserState* state) {
    static char line[MAX_LINE_LENGTH]; //static so that it is not only local to the function and can be returned
    int i = 0;
    char c;
    
    while ((c = read_char(state)) && c != '\n' && c != '\0' && i < MAX_LINE_LENGTH - 1) { //WHile we don't reach end of line or EOF
        line[i] = c;
        i++;
    }
    line[i] = '\0';
    
    return line;
}

// Main recursive parsing function
// Returns: index of stop_symbol that was found (0-based), or -1 if EOF reached
int parse_until(ParserState* state, const char** stop_symbols, bool copy_to_output) {
    char c;
    bool at_line_start = true;
    
    while ((c = read_char(state)) != '\0') {    //repeat until the end of the file (read_char returns '\0' when it reaches it)
        // Check for directives at line start
        if (at_line_start && c == '#' && state->process_directives) {
            //skip the spaces we may find between and read the following word
            skip_whitespace(state);
            char* directive = read_word(state);
            
            //detect the directive read and call the pertinent module to handle it
            if (directive) {
                // Process include
                if (strcmp(directive, "include") == 0) {
                    process_include(state, copy_to_output);
                    at_line_start = true;
                    continue;
                }
                // Process define
                else if (strcmp(directive, "define") == 0) {
                    if (copy_to_output) {
                        process_define(state);
                    } else {
                        // Still parse it but don't add to dictionary, make the parser advance
                        read_line(state);
                    }
                    at_line_start = true;
                    continue;
                }
                // Process ifdef/ifndef
                else if (strcmp(directive, "ifdef") == 0 || strcmp(directive, "ifndef") == 0) {
                    //store which one was found to pass it to the module
                    bool is_ifndef = (strcmp(directive, "ifndef") == 0);
                    process_ifdef(state, is_ifndef, copy_to_output);
                    at_line_start = true;
                    continue;
                }
                
                // Check for stop symbols
                // Iterate through the array of stop symbols
                for (int i = 0; stop_symbols[i] != NULL; i++) {
                    if (strcmp(directive, stop_symbols[i]) == 0) {
                        // Consume rest of line
                        read_line(state);
                        return i; // Return the index of the matched stop symbol
                    }
                }
                
                // Special handling for endif when not in stop_symbols
                // This provides backward compatibility and error checking
                if (strcmp(directive, "endif") == 0) {
                    // Check if "endif" is in stop_symbols
                    bool endif_allowed = false;
                    for (int i = 0; stop_symbols[i] != NULL; i++) {
                        if (strcmp(stop_symbols[i], "endif") == 0) {
                            endif_allowed = true;
                            break;
                        }
                    }
                    
                    if (!endif_allowed) {
                        report_error(ERROR_ERROR, state->current_filename, state->current_line,
                                   "Unexpected #endif without matching #ifdef");
                    }
                    read_line(state);
                    // Don't return here since we want to find it in the loop above
                }
            }
            at_line_start = false;
            continue;
        }
        
        // Check for comments
        if (c == '/') {
            char next = peek_char(state);
            //checking if it is a comment and call the module to process it
            if (next == '/' || next == '*') {
                int comment_result = process_comment(state, c, next, copy_to_output);
                at_line_start = (next == '/'); // Single-line comment ends at newline
                continue;
            }
        }
        
        // Handle strings (don't process macros inside strings)
        if (c == '"') {
            if (copy_to_output) { // Copy the quote character to the output if output is enabled
                fputc(c, state->output_file);
            }
            state->in_string = !state->in_string; //canviem estat, no s’han de substituir macros dins de cadenes
            at_line_start = false; // A quote can never be at the start of a line for directives
            continue;
        }
        
        // Check for identifiers (potential macros)
        // Only when not inside a string
        if (!state->in_string && (isalpha(c))) {
            unread_char(state, c); //read last work, put the character back
            char* word = read_word(state);
            
            if (word) {
                // Try to substitute macro
                char* substitution = substitute_macro(state, word);
                if (substitution && copy_to_output) {
                    fprintf(state->output_file, "%s", substitution); //macro found
                } else if (copy_to_output) {
                    fprintf(state->output_file, "%s", word); //not a macro
                }
            }
            at_line_start = false;
            continue;
        }
        
        // Regular character - copy if needed
        if (copy_to_output) {
            fputc(c, state->output_file);
        }
        
        at_line_start = (c == '\n'); // Used to detect preprocessor directives (#), need to be at  line start
    }
    
    // Reached EOF without finding any stop symbol
    return -1;
}


