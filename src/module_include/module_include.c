// Gorka Hernández Villalón

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "./module_include.h"
#include "../module_parser/module_parser.h"
#include "../module_errors/module_errors.h"

#define MAX_INCLUDE_PATH 512
#define MAX_INCLUDE_DEPTH 64

static int include_depth = 0;

void module_include_run(void) {
    printf("Loaded module_include: include directive processing module\n");
}

static int read_filename(ParserState* state, char* filename, size_t max_len, char delimiter) {
    size_t i = 0;
    char c;
    
    while ((c = read_char(state)) != '\0' && c != '\n') {
        if (c == delimiter) {
            filename[i] = '\0';
            return 1;
        }
        
        if (i < max_len - 1) {
            filename[i++] = c;
        } else {
            report_error(ERROR_ERROR, state->current_filename, state->current_line,
                        "Include filename too long");
            return 0;
        }
    }    
    report_error(ERROR_ERROR, state->current_filename, state->current_line,
                "Malformed #include directive");
    return 0;
}

static int open_include(ParserState* state, const char* filename, bool copy_to_output) {
    FILE* include_file = fopen(filename, "r");
    if (!include_file) {
        char alt_path[MAX_INCLUDE_PATH];
        const char* last_slash = strrchr(state->current_filename, '/');
        if (last_slash) {
            size_t dir_len = last_slash - state->current_filename + 1;
            if (dir_len + strlen(filename) < MAX_INCLUDE_PATH) {
                strncpy(alt_path, state->current_filename, dir_len);
                alt_path[dir_len] = '\0';
                strcat(alt_path, filename);
                include_file = fopen(alt_path, "r");
            }
        }
        if (!include_file) {
            report_error(ERROR_ERROR, state->current_filename, state->current_line,
                        "Cannot open included file");
            return 0;
        }
    }
    if (copy_to_output && state->output_file) {
        fprintf(state->output_file, "\n");
    }
    char c;
    while ((c = fgetc(include_file)) != EOF) {
        if (copy_to_output && state->output_file) {
            fputc(c, state->output_file);
        }
    }
    if (copy_to_output && state->output_file) {
        fprintf(state->output_file, "\n");
    }
    fclose(include_file);
    return 1;
}

int process_include(ParserState* state, bool copy_to_output) {
    skip_whitespace(state);
    char c = read_char(state);
    char delimiter;
    if (c == '"') {
        delimiter = '"';
    } else if (c == '<') {
        delimiter = '>';
    } else {
        report_error(ERROR_ERROR, state->current_filename, state->current_line,
                    "Invalid #include syntax");
        return -1;
    }
    char filename[MAX_INCLUDE_PATH];
    if (!read_filename(state, filename, MAX_INCLUDE_PATH, delimiter)) {
        return -1;
    }
    char rest;
    while ((rest = read_char(state)) != '\0' && rest != '\n') {
    }
    if (include_depth >= MAX_INCLUDE_DEPTH) {
        report_error(ERROR_ERROR, state->current_filename, state->current_line,
                    "Maximum include depth exceeded");
        return -1;
    }
    include_depth++;
    if (copy_to_output) {
        open_include(state, filename, copy_to_output);
    }
    include_depth--;
    return 0;
}