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
static void store_entry(const char* message, int line, Severity severity) {
    const char* prefix = (severity == SEVERITY_ERROR) ? "ERROR" : "WARNING";
    const char* filename = (status.ifile_name[0] != '\0') ? status.ifile_name : "<unknown>";
    fprintf(stderr, "%s: %s:%d: %s\n", prefix, filename, line, message);
    if (entry_count < MAX_ERRORS) {
        strncpy(error_list[entry_count].message, message, MAX_LINE_LENGTH - 1);
        error_list[entry_count].message[MAX_LINE_LENGTH - 1] = '\0';
        error_list[entry_count].line = line;
        error_list[entry_count].severity = severity;
        entry_count++;
    }
    if (severity == SEVERITY_ERROR) {
        total_errors++;
    } else {
        total_warnings++;
    }
}
void report_error(const char* message, int line) {
    store_entry(message, line, SEVERITY_ERROR);
}
void report_warning(const char* message, int line) {
    store_entry(message, line, SEVERITY_WARNING);
}
void report_error_token(const Token* token, int line) {
    char msg[MAX_LINE_LENGTH];
    snprintf(msg, sizeof(msg), "Non-recognized token '%s'", token->lexeme);
    store_entry(msg, line, SEVERITY_ERROR);
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
    fprintf(stderr, "--\n");
}