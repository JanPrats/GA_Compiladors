#ifndef MODULE_ERROR_H
#define MODULE_ERROR_H
#include "../config.h"
// Maximum number of error/warning entries stored
#define MAX_ERRORS 256
// Error severity levels
typedef enum {
    SEVERITY_WARNING,
    SEVERITY_ERROR
} Severity;
// Single error/warning entry
typedef struct {
    char message[MAX_LINE_LENGTH];
    int line;
    Severity severity;
    Error error_type;
} ErrorEntry;
void error_init(void);
void report_error(const char* message, int line);
void report_warning(const char* message, int line);
void report_error_token(const Token* token, int line);
void report_error_typed(Error error_type, int line);
int error_count(void);
int warning_count(void);
void error_finalize(void);
const char* error_type_to_string(Error err);
#endif // MODULE_ERROR_H