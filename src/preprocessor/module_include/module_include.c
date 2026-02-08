/*
 * -----------------------------------------------------------------------------
 * module_include.c
 *
 * This module provides functionality to handle #include directives in the parser.
 * It has been refactored to support recursive preprocessing, allowing included
 * files to contain their own preprocessor directives.
 *
 * Key changes:
 * - `process_include` now saves the current parser state, switches to the 
 *   included file, and calls `parse_until` recursively.
 * - Supports relative path searching for included files.
 * - Prevents infinite recursion with a depth limit.
 *
 * Authors: Gorka Hernández Villalón
 * -----------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "module_include.h"
#include "../module_parser/module_parser.h"
#include "../module_errors/module_errors.h"

#define MAX_INCLUDE_PATH 512
#define MAX_INCLUDE_DEPTH 64

static int include_depth = 0;

void module_include_run(void) {
    printf("Loaded module_include: recursive include directive processing module\n");
}

int process_include(ParserState* state, bool copy_to_output) {
    // Skip whitespace after #include
    skip_whitespace(state);
    
    char c = read_char(state);
    char delimiter;
    if (c == '"') {
        delimiter = '"';
    } else if (c == '<') {
        delimiter = '>';
    } else {
        report_error(ERROR_ERROR, state->current_filename, state->current_line,
                    "Invalid #include syntax: expected \" or <");
        // Consume till newline
        while ((c = read_char(state)) && c != '\n');
        return -1;
    }
    
    char filename[MAX_INCLUDE_PATH];
    int i = 0;
    while ((c = read_char(state)) && c != delimiter && c != '\n' && i < MAX_INCLUDE_PATH - 1) {
        filename[i++] = c;
    }
    filename[i] = '\0';
    
    if (c != delimiter) {
        report_error(ERROR_ERROR, state->current_filename, state->current_line,
                    "Malformed #include directive: missing closing delimiter");
        return -1;
    }
    
    // Consume rest of line to avoid issues in the outer parser
    while ((c = peek_char(state)) && c != '\n') {
        read_char(state);
    }
    // Read the newline too if it exists
    if (peek_char(state) == '\n') {
        read_char(state);
    }

    if (include_depth >= MAX_INCLUDE_DEPTH) {
        report_error(ERROR_ERROR, state->current_filename, state->current_line,
                    "Maximum include depth exceeded");
        return -1;
    }

    // Try to open include file
    FILE* include_fp = fopen(filename, "r");
    char actual_path[MAX_INCLUDE_PATH];
    strncpy(actual_path, filename, MAX_INCLUDE_PATH - 1);
    actual_path[MAX_INCLUDE_PATH - 1] = '\0';

    if (!include_fp) {
        // Try relative to current file's directory
        const char* last_slash = strrchr(state->current_filename, '/');
        if (last_slash) {
            size_t dir_len = last_slash - state->current_filename + 1;
            if (dir_len + strlen(filename) < MAX_INCLUDE_PATH) {
                char temp_path[MAX_INCLUDE_PATH];
                strncpy(temp_path, state->current_filename, dir_len);
                temp_path[dir_len] = '\0';
                strcat(temp_path, filename);
                include_fp = fopen(temp_path, "r");
                if (include_fp) {
                    strcpy(actual_path, temp_path);
                }
            }
        }
    }

    if (!include_fp) {
        char error_msg[MAX_LINE_LENGTH];
        snprintf(error_msg, sizeof(error_msg), "Cannot open include file '%s'", filename);
        report_error(ERROR_ERROR, state->current_filename, state->current_line, error_msg);
        return -1;
    }

    // Recursion preparation: Save state
    FILE* old_file = state->current_file;
    char old_filename[MAX_FILENAME];
    strncpy(old_filename, state->current_filename, MAX_FILENAME - 1);
    old_filename[MAX_FILENAME - 1] = '\0';
    int old_line = state->current_line;
    bool old_has_lookahead = state->has_lookahead;
    char old_lookahead = state->lookahead;

    // Set new state for recursion
    state->current_file = include_fp;
    strncpy(state->current_filename, actual_path, MAX_FILENAME - 1);
    state->current_filename[MAX_FILENAME - 1] = '\0';
    state->current_line = 1;
    state->has_lookahead = false;

    include_depth++;
    
    // Add a newline before included content to separate from #include line
    if (copy_to_output && state->output_file) {
        fputc('\n', state->output_file);
    }

    // RECURSIVE CALL
    // stop_symbols = NULL means run until EOF of the CURRENT state->current_file
    parse_until(state, NULL, copy_to_output);

    // Add a newline after included content
    if (copy_to_output && state->output_file) {
        fputc('\n', state->output_file);
    }

    include_depth--;

    // Cleanup and Restore
    fclose(include_fp);
    state->current_file = old_file;
    strncpy(state->current_filename, old_filename, MAX_FILENAME - 1);
    state->current_filename[MAX_FILENAME - 1] = '\0';
    state->current_line = old_line;
    state->has_lookahead = old_has_lookahead;
    state->lookahead = old_lookahead;

    return 0;
}