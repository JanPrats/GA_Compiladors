/*
 * -----------------------------------------------------------------------------
 * module_errors.h
 *
 * Interface for the error handling module.
 *
 * This header defines the ErrorLevel types and prototypes for initializing,
 * reporting, and summarizing errors during the preprocessing phase.
 *
 * Author: Andrea Salló Ribas
 * -----------------------------------------------------------------------------
 */

#ifndef MODULE_ERRORS_H
#define MODULE_ERRORS_H

#include "../main.h"
#include <stdio.h>

/* Error severity levels */
typedef enum {
    ERROR_WARNING, //not ideal but not critical. NOT COUNT AS ERROR IN THE COUNTER
    ERROR_ERROR //incorrect for the preprocesor. The code is not following the rules. COUNT AS ERROR IN THE COUNTER
} ErrorLevel;

/* Initializes the error system */
void errors_init(void);

/* Report an error or warning */
void report_error(
    ErrorLevel level,
    const char *filename,
    int line,
    const char *message
);

/* Returns the number of errors detected */
int errors_count(void);

/* Called at the end of preprocessing */
void errors_finalize(void);

#endif

