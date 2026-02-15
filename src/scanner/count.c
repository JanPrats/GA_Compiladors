#include "count.h"
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Global counting state
CountConfig global_count_config = {0};

// Flag to track if counting system has been initialized
static bool count_initialized = false;

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
        //create dbgcnt file
        global_count_config.countifle = fopen("dbgcnt", "w"); //per ara així pero s'ha de mirar el nom
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
    
    // Increment the appropriate counter
    if (strcmp(counter_type, "COMP") == 0) {
        global_count_config.countvariables.countcomp += amount;
    } else if (strcmp(counter_type, "IO") == 0) {
        global_count_config.countvariables.countio += amount;
    } else if (strcmp(counter_type, "GEN") == 0) {
        global_count_config.countvariables.countgen += amount;
    } else {
        //cridar modul error per error de tipus de comptador
        return;
    }
    
    FILE* out = global_count_config.countifle ? global_count_config.countifle : stdout;
    
    // For simplicity, partial counts = total counts (we don't track per-function state yet)
    // This could be enhanced later with a hash map to track per-function counters
    fprintf(out, "[COUNT] line=%-4d func=%-30s +%-4d %-4s | partial: comp=%-6d io=%-6d gen=%-6d | total: comp=%-6d io=%-6d gen=%-6d\n",
            line,
            func_name,
            amount,
            counter_type,
            global_count_config.countvariables.countcomp,  // For now, partial = total
            global_count_config.countvariables.countio,
            global_count_config.countvariables.countgen,
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
    
    FILE* out = global_count_config.countifle ? global_count_config.countifle : stdout;
    
    fprintf(out, "[COUNT] ==========================================\n");
    fprintf(out, "[COUNT] === Final Operation Count Summary ===\n");
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
    global_count_config.countifle = NULL;
}
