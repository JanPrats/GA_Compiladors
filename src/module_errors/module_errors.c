/*
 * -----------------------------------------------------------------------------
 * module_errors.c
 *
 * Error handling module for the simplified C preprocessor.
 *
 * This module centralizes the detection, reporting, and tracking of errors
 * and warnings produced during the preprocessing phase.
 *
 * Responsibilities:
 *  - Provide informative error and warning messages to stderr.
 *  - Include the source filename and line number where the error occurred.
 *  - Allow the preprocessing to continue after errors whenever possible.
 *  - Accumulate multiple errors in a single execution.
 *  - Offer a reusable and extensible error handling interface that can be
 *    shared across different preprocessing modules (ifdef, macros, args, etc.)
 *    and potentially reused in later compiler phases.
 *
 * Main functions:
 *  - errors_init(): Initializes the internal error state.
 *  - report_error(): Reports a warning or error with file and line information.
 *  - errors_count(): Returns the total number of errors detected.
 *  - errors_finalize(): Prints a summary at the end of preprocessing.
 *
 * Design notes:
 *  - This module does NOT decide when to stop the program.
 *  - The decision to terminate is delegated to the main program or parser.
 *  - The module is designed to be easily extended to support new error types.
 *
 * Usage:
 *  - Called by parser and preprocessing modules when an error condition
 *    is detected.
 *  - Will be implemented:
 *        Error 1: #endif without matching #ifdef (during parsing - will go on module_ifdef because this error it can be just detected by the modul who proces #ifdef/#endif)
 *        Error 2: #ifdef without matching #endif (at the end of the file - will go on the parser because we just can detect the error when we finished reading ALL the file)
 *
 * Author: Andrea Salló Ribas
 * -----------------------------------------------------------------------------
 */

#include <stdio.h>
#include "./module_errors.h"

void module_errors_run(void) {
    printf("Loaded module_errors: error and warning detection module\n");
}

/* Internal error counter */
static int error_counter = 0;

void errors_init(void) {
    error_counter = 0;
}

void report_error(
    ErrorLevel level,
    const char *filename,
    int line,
    const char *message
) {
    if (level == ERROR_WARNING) {
        fprintf(stderr,
                "WARNING: %s:%d: %s\n",
                filename,
                line,
                message);
    } else {
        fprintf(stderr,
                "ERROR: %s:%d: %s\n",
                filename,
                line,
                message);
        error_counter++;
    }
}

int errors_count(void) {
    return error_counter;
}

void errors_finalize(void) {
    if (error_counter > 0) {
        fprintf(stderr,
                "\nPreprocessing finished with %d error(s).\n",
                error_counter);
    } else {
        fprintf(stderr,
                "\nPreprocessing finished successfully. No errors detected.\n");
    }
}

