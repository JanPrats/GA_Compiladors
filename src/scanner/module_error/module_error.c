/*
 * -----------------------------------------------------------------------------
 * module_error.c
 *
 * Error handling module for the Scanner (Lexical Analyzer).
 * Tracks errors and warnings during scanning, prints them to stderr,
 * and provides a summary at the end.
 *
 * Error format:   ERROR: <filename>:<line>: <message>
 * Warning format: WARNING: <filename>:<line>: <message>
 *
 * Team: GA
 * Contributor/s: Gorka Hernández Villalón
 * -----------------------------------------------------------------------------
 */
#include "module_error.h"
#include <stdio.h>
#include <string.h>
// Internal state
static ErrorEntry error_list[MAX_ERRORS];
static int entry_count = 0;
static int total_errors = 0;
static int total_warnings = 0;
extern Status status;
void error_init(void) {
    entry_count = 0;
    total_errors = 0;
    total_warnings = 0;
    memset(error_list, 0, sizeof(error_list));
}
static void store_entry(const char* message, int line, Severity severity, Error error_type, Step step) {
    const char* prefix = (severity == SEVERITY_ERROR) ? "ERROR" : "WARNING";
    const char* step_str = step_to_string(step);
    const char* filename = (status.ifile_name[0] != '\0') ? status.ifile_name : "<unknown>";
    fprintf(stderr, "[%s] %s: %s:%d: %s\n", step_str, prefix, filename, line, message);
    if (status.debug && status.ofile) {
        fprintf(status.ofile, "[%s] %s: %s:%d: %s\n", step_str, prefix, filename, line, message);
    }
    if (entry_count < MAX_ERRORS) {
        strncpy(error_list[entry_count].message, message, MAX_LINE_LENGTH - 1);
        error_list[entry_count].message[MAX_LINE_LENGTH - 1] = '\0';
        error_list[entry_count].line = line;
        error_list[entry_count].severity = severity;
        error_list[entry_count].error_type = error_type;
        error_list[entry_count].step = step;
        entry_count++;
    }
    if (severity == SEVERITY_ERROR) {
        total_errors++;
    } else {
        total_warnings++;
    }
}
void report_error(const char* message, int line, Step step) {
    store_entry(message, line, SEVERITY_ERROR, ERR_NONE, step);
}
void report_warning(const char* message, int line, Step step) {
    store_entry(message, line, SEVERITY_WARNING, ERR_NONE, step);
}
void report_error_token(const Token* token, int line, Step step) {
    char msg[MAX_LINE_LENGTH];
    snprintf(msg, sizeof(msg), "Non-recognized token '%s'", token->lexeme);
    store_entry(msg, line, SEVERITY_ERROR, ERR_TOKEN_NOT_RECOGNIZED, step);
}
void report_error_typed(Error error_type, int line, Step step) {
    const char* msg = error_type_to_string(error_type);
    store_entry(msg, line, SEVERITY_ERROR, error_type, step);
}
int error_count(void) {
    return total_errors;
}
int warning_count(void) {
    return total_warnings;
}
void error_finalize(void) {
    fprintf(stderr, "\n--- Scanner Summary ---\n");
    fprintf(stderr, "Total errors:   %d\n", total_errors);
    fprintf(stderr, "Total warnings: %d\n", total_warnings);
    if (total_errors == 0 && total_warnings == 0) {
        fprintf(stderr, "Scanning completed successfully.\n");
    } else {
        fprintf(stderr, "Scanning completed with issues.\n");
    }
    fprintf(stderr, "---\n");
    if (status.debug && status.ofile) {
        fprintf(status.ofile, "\n--- Scanner Summary ---\n");
        fprintf(status.ofile, "Total errors:   %d\n", total_errors);
        fprintf(status.ofile, "Total warnings: %d\n", total_warnings);
        if (total_errors == 0 && total_warnings == 0) {
            fprintf(status.ofile, "Scanning completed successfully.\n");
        } else {
            fprintf(status.ofile, "Scanning completed with issues.\n");
        }
        fprintf(status.ofile, "---\n");
    }
}
const char* step_to_string(Step step) {
    switch (step) {
        case SCANNER_STEP:  return "SCANNER";
        case PARSER_STEP:   return "PARSER";
        default:            return "UNKNOWN_STEP";
    }
}
const char* error_type_to_string(Error err) {
    switch (err) {
        case ERR_NONE:                return "NONE";
        case ERR_INVALID_ARGUMENT:    return "INVALID_ARGUMENT";
        case ERR_FILE_NOT_FOUND:      return "FILE_NOT_FOUND";
        case ERR_TOKEN_NOT_RECOGNIZED: return "TOKEN_NOT_RECOGNIZED";
        case ERR_TOKEN_TOO_LONG:      return "TOKEN_TOO_LONG";
        case ERR_UNTERMINATED_LITERAL: return "UNTERMINATED_LITERAL";
        case ERR_MAX_TOKENS_EXCEEDED: return "MAX_TOKENS_EXCEEDED";
        case ERR_EMPTY_FILE:          return "EMPTY_FILE";
        default:                      return "UNKNOWN_ERROR";
    }
}