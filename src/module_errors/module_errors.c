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

