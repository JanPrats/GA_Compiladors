// -----------------------------------------------------------------------------
// Jan Prats Soler
// Module: module_comments_remove
//
// This module is responsible for detecting and processing C-style comments.
// It supports:
//   - Single-line comments (//)
//   - Multi-line comments (/* ... */)
//
// Depending on the parser configuration, comments can be:
//   - Removed entirely
//   - Preserved and copied to the output (if flag -c == TRU)
// -----------------------------------------------------------------------------

#include <stdio.h>
#include <stdbool.h>

#include "./module_comments_remove.h"
#include "../module_parser/parser.h"
#include "../module_errors/module_errors.h"

// -----------------------------------------------------------------------------
// process_comment
//
// Detects and processes a comment starting at the current parser position.
//
// Parameters:
//   state           - Pointer to the current parser state
//   current_char    - Current character being analyzed
//   next_char       - Lookahead character (next in input stream)
//   copy_to_output  - Indicates whether output writing is enabled
//
// Returns:
//   1 if a comment was detected and processed
//   0 if the current characters do not form a comment
// -----------------------------------------------------------------------------
int process_comment(ParserState* state, char current_char, char next_char, bool copy_to_output) {

    // -------------------------------------------------------------------------
    // Single-line comment: //
    // -------------------------------------------------------------------------
    if (current_char == '/' && next_char == '/') {

        // Consume the second '/'
        read_char(state);

        // Write the comment start only if comments are not being removed
        if (!state->remove_comments && copy_to_output) {
            fprintf(state->output_file, "//");
        }

        // Read characters until end-of-line or end-of-file
        char c;
        while ((c = read_char(state)) && c != '\n' && c != '\0') {
            if (!state->remove_comments && copy_to_output) {
                fputc(c, state->output_file);
            }
        }

        // Preserve the newline character if it exists
        if (c == '\n') {
            if (!state->remove_comments && copy_to_output) {
                fputc('\n', state->output_file);
            }
        }

        return 1; // Comment processed
    }

    // -------------------------------------------------------------------------
    // Multi-line comment: /* ... */
    // -------------------------------------------------------------------------
    if (current_char == '/' && next_char == '*') {

        // Consume the '*'
        read_char(state);

        // Write the comment start if comments are preserved
        if (!state->remove_comments && copy_to_output) {
            fprintf(state->output_file, "/*");
        }

        // Read characters until the closing sequence "*/" is found
        char prev = '\0';
        char c;

        while ((c = read_char(state)) != '\0') {

            // Detect end of multi-line comment
            if (prev == '*' && c == '/') {
                if (!state->remove_comments && copy_to_output) {
                    fputc('/', state->output_file);
                }
                return 1; // Comment processed
            }

            // Copy comment content if required
            if (!state->remove_comments && copy_to_output) {
                fputc(c, state->output_file);
            }

            prev = c;
        }

        // If end-of-file is reached without closing the comment, report a warning
        report_error(ERROR_WARNING,
                 state->current_filename,
                 state->current_line,
                 "Unclosed multi-line comment");

        return 1; // Comment was detected, even if malformed
    }

    // -------------------------------------------------------------------------
    // Not a comment
    // -------------------------------------------------------------------------
    return 0;
}
