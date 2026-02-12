#ifndef COUNT_H
#define COUNT_H

#include "config.h"
#include <stdbool.h>

extern CountConfig global_count_config;

void count_init(const char* ifile_name, bool countout_flag);
void count_increment(const char* func_name, int line, int amount, const char* counter_type);
void count_finalize(void);

// Preprocessor macros for conditional counting
// When COUNTCONFIG_F is true, these expand to actual function calls
// If the flag is false the functions are empty and do not add overhead

#if COUNTCONFIG_F

    #define COUNT_INIT() count_init(status.ifile_name, COUNTOUT_F)
    
    #define COUNT_FINALIZE() count_finalize()
    
    //function to count comparisons
    #define COUNT_COMP(n) count_increment(__func__, status.line, (n), "COMP")
    
    //function to count I/O operations
    #define COUNT_IO(n) count_increment(__func__, status.line, (n), "IO")
    
    //function to count general instructions
    #define COUNT_GEN(n) count_increment(__func__, status.line, (n), "GEN")
    
    //function to report a count increment with full context
    #define COUNT_REPORT(func_name, amount, counter_type) \
        count_increment((func_name), status.line, (amount), (counter_type))

#else
    // When counting is disabled, all macros expand to nothing
    #define COUNT_INIT() ((void)0)
    #define COUNT_FINALIZE() ((void)0)
    #define COUNT_COMP(n) ((void)0)
    #define COUNT_IO(n) ((void)0)
    #define COUNT_GEN(n) ((void)0)
    #define COUNT_REPORT(func_name, amount, counter_type) ((void)0)

#endif // COUNTCONFIG_F

#endif // COUNT_H
