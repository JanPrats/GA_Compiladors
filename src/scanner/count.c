/*
 * -----------------------------------------------------------------------------
 * count.c
 *
 * Operation counting module for performance analysis and debugging.
 * Tracks and counts different types of operations (comparisons, I/O, general)
 * during the scanning process for performance profiling.
 *
 * Key Features:
 * - Initialize and finalize counting system
 * - Track operation counts by type and location
 * - Support for separate debug count file or main output
 * - Operation counting macros for conditional compilation
 * - Counter statistics output
 *
 * Supported Counter Types:
 * - COMP: Comparison operations
 * - IO: Input/Output operations
 * - GEN: General instructions
 *
 * Team: GA
 * Contributor/s: Marc Rodríguez
 * -----------------------------------------------------------------------------
 */

#include "count.h"
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Global counting state
CountConfig global_count_config = {0};

// Flag to track if counting system has been initialized
static bool count_initialized = false;

// Per-function counting tracking
#define MAX_FUNCTIONS 100

typedef struct {
    char name[MAX_FUNCTION_NAME];
    CountVars counts;
    bool active;
} FunctionRecord;

static FunctionRecord function_records[MAX_FUNCTIONS] = {0};
static int num_functions = 0;

/**
 * Find or create a function record for tracking per-function counts
 * @param func_name Name of the function
 * @return Pointer to the function record, or NULL if table is full
 */
static FunctionRecord* get_function_record(const char* func_name) {
    // Search for existing record
    for (int i = 0; i < num_functions; i++) {
        if (strcmp(function_records[i].name, func_name) == 0) {
            return &function_records[i];
        }
    }
    
    // Create new record if space available
    if (num_functions < MAX_FUNCTIONS) {
        FunctionRecord* record = &function_records[num_functions++];
        strncpy(record->name, func_name, MAX_FUNCTION_NAME - 1);
        record->name[MAX_FUNCTION_NAME - 1] = '\0';
        record->counts.countcomp = 0;
        record->counts.countio = 0;
        record->counts.countgen = 0;
        record->active = true;
        return record;
    }
    
    return NULL; // Table full
}

/**
 * Initialize the counting system
 * @param ifile_name Input file name (used to create dbgcnt file name)
 * @param countout_flag Where to send count output (OUT=1 or DBGCOUNT=0)
 */
void count_init(const char* ifile_name, bool countout_flag) {
    if (status.oform != DEBUG) return;

    if (count_initialized) {
        fprintf(stderr, "[COUNT WARNING] count_init called twice, skipping re-initialization\n");
        return;
    }
    
    //init all counters
    global_count_config.countvariables.countcomp = 0;
    global_count_config.countvariables.countio = 0;
    global_count_config.countvariables.countgen = 0;
    global_count_config.countout = countout_flag;
    global_count_config.countifle = NULL;
    
    //where to send output
    if (countout_flag == DBGCOUNT) {
        // Create dbgcnt filename
        char dbgcnt_filename[MAX_FILENAME];
        snprintf(dbgcnt_filename, MAX_FILENAME, "%sdbgcnt", ifile_name);
        printf("[COUNT] Debug count file: %s\n", dbgcnt_filename);
        
        // Open the dbgcnt file for writing
        global_count_config.countifle = fopen(dbgcnt_filename, "w");
        if (global_count_config.countifle == NULL) {
            fprintf(stderr, "[COUNT ERROR] Could not open '%s', using stderr instead\n", dbgcnt_filename);
            global_count_config.countifle = stderr;
        } else {
            fprintf(stderr, "[COUNT] Created debug count file: %s\n", dbgcnt_filename);
        }
    } else {
        // OUT mode: use the main output file
        global_count_config.countifle = status.ofile;
    }
    
    count_initialized = true;
    
    // Print initialization message
    FILE* out = global_count_config.countifle ? global_count_config.countifle : stdout;
    fprintf(out, "[COUNT] === Operation Counting Initialized ===\n");
    fprintf(out, "[COUNT] Mode: %s\n", countout_flag == DBGCOUNT ? "DBGCOUNT (separate file)" : "OUT (main output)");
    fprintf(out, "[COUNT] Format: line=<num> func=<name> +<amount> <type> | partial: comp io gen | total: comp io gen\n");
    fprintf(out, "[COUNT] ==========================================\n");
    fflush(out);
}

/**
 * Increment a counter and optionally report the increment
 * @param func_name Name of the function where counting occurs
 * @param line Line number in source file
 * @param amount Amount to increment
 * @param counter_type Type of counter: "COMP", "IO", or "GEN"
 */
void count_increment(const char* func_name, int line, int amount, const char* counter_type) {
    if (!count_initialized) {
        fprintf(stderr, "[COUNT ERROR] count_increment called before count_init\n");
        return;
    }
    
    // Get or create function record for per-function tracking
    FunctionRecord* func_record = get_function_record(func_name);
    
    // Increment the appropriate global counter
    if (strcmp(counter_type, "COMP") == 0) {
        global_count_config.countvariables.countcomp += amount;
        if (func_record) func_record->counts.countcomp += amount;
    } else if (strcmp(counter_type, "IO") == 0) {
        global_count_config.countvariables.countio += amount;
        if (func_record) func_record->counts.countio += amount;
    } else if (strcmp(counter_type, "GEN") == 0) {
        global_count_config.countvariables.countgen += amount;
        if (func_record) func_record->counts.countgen += amount;
    } else {
        fprintf(stderr, "[COUNT ERROR] Unknown counter type: %s\n", counter_type);
        return;
    }
    
    // Format and output the count report
    FILE* out = global_count_config.countifle ? global_count_config.countifle : stdout;
    
    // Partial counts = per-function totals, Total counts = global cumulative
    fprintf(out, "[COUNT] line=%-4d func=%-30s +%-4d %-4s | partial: comp=%-6d io=%-6d gen=%-6d | total: comp=%-6d io=%-6d gen=%-6d\n",
            line,
            func_name,
            amount,
            counter_type,
            func_record ? func_record->counts.countcomp : 0,
            func_record ? func_record->counts.countio : 0,
            func_record ? func_record->counts.countgen : 0,
            global_count_config.countvariables.countcomp,
            global_count_config.countvariables.countio,
            global_count_config.countvariables.countgen);
    
    fflush(out);
}

/**
 * Finalize the counting system and print summary
 */
void count_finalize(void) {
    if (status.oform != DEBUG) return;

    if (!count_initialized) {
        fprintf(stderr, "[COUNT WARNING] count_finalize called but counting was not initialized\n");
        return;
    }
    
    FILE* out = global_count_config.countifle ? global_count_config.countifle : stdout;
    
    fprintf(out, "[COUNT] ==========================================\n");
    fprintf(out, "[COUNT] === Per-Function Statistics ===\n");
    
    // Report per-function counts
    for (int i = 0; i < num_functions; i++) {
        FunctionRecord* rec = &function_records[i];
        int func_total = rec->counts.countcomp + rec->counts.countio + rec->counts.countgen;
        fprintf(out, "[COUNT] %-30s | comp=%-6d io=%-6d gen=%-6d | total=%-6d\n",
                rec->name,
                rec->counts.countcomp,
                rec->counts.countio,
                rec->counts.countgen,
                func_total);
    }
    
    fprintf(out, "[COUNT] ==========================================\n");
    fprintf(out, "[COUNT] === Global Summary ===\n");
    fprintf(out, "[COUNT] Total Comparisons (COMP): %d\n", global_count_config.countvariables.countcomp);
    fprintf(out, "[COUNT] Total I/O Operations (IO): %d\n", global_count_config.countvariables.countio);
    fprintf(out, "[COUNT] Total General Instr (GEN): %d\n", global_count_config.countvariables.countgen);
    fprintf(out, "[COUNT] ==========================================\n");
    fprintf(out, "[COUNT] GRAND TOTAL: %d operations\n", 
            global_count_config.countvariables.countcomp +
            global_count_config.countvariables.countio +
            global_count_config.countvariables.countgen);
    fprintf(out, "[COUNT] ==========================================\n");
    
    fflush(out);
    
    // Close the dbgcnt file if it was opened (and it's not stderr)
    if (global_count_config.countout == DBGCOUNT && 
        global_count_config.countifle != NULL && 
        global_count_config.countifle != stderr && 
        global_count_config.countifle != stdout) {
        fclose(global_count_config.countifle);
        fprintf(stderr, "[COUNT] Debug count file closed\n");
    }
    
    // Reset state
    count_initialized = false;
    num_functions = 0;
    memset(function_records, 0, sizeof(function_records));
    global_count_config.countifle = NULL;
}
