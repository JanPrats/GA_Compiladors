/*
 * -----------------------------------------------------------------------------
 * main.c
 *
 * Main entry point for the C scanner application. This program processes
 * C source files by performing lexical analysis and tokenization using
 * finite automata (DFA) to recognize various token categories.
 *
 * Program Flow:
 * 1. Initialize error tracking system
 * 2. Process command-line arguments (input/output files)
 * 3. Initialize automata for token recognition
 * 4. Run automata driver to scan the input file and generate tokens
 * 5. Close files and finalize error reporting
 * 6. Exit with appropriate status code based on error count
 *
 * Token Categories Recognized:
 * - Numbers (integers, floats)
 * - Keywords (if, while, for, etc.)
 * - Types (int, char, float, etc.)
 * - Identifiers (variable/function names)
 * - Operators (+, -, *, /, etc.)
 * - Special characters ({, }, (, ), etc.)
 * - String/Character literals
 * - Non-recognized characters
 *
 * Usage:
 *     ./scanner <input_file> [-help]
 *     Output file: <input_file>scn
 *     Use -help flag for detailed usage information
 *
 * Exit Codes:
 *     0 - Success (scanning completed, may have non-critical errors)
 *     1 - Failure (critical errors occurred during scanning)
 *
 * Team: GA
 * Contributor/s:
 * -----------------------------------------------------------------------------
 */

#include "./main.h"
#include "config.h"
#include "module_init/module_init.h"

int main(int argc, char *argv[]) {
    //errors_init();
    
    int returned = init_program(argc, argv);

    // if (returned == HELP_RETURN) { //Show help if requested
    //     show_help();
    //     return 0;
    // }

    if (returned != CORRECT_RETURN) {
        fprintf(stderr, "Error: Failed to initialize scanner\n");
        return 1;
    }

    // Initialize automata for token recognition
    AutomataList automata_list;
    init_automata(&automata_list);

    // Run automata driver to scan input and generate tokens
    automata_driver(automata_list.automatas, automata_list.num_automata);

    // Close files
    if (status.ifile) {
        fclose(status.ifile);
    }
    if (status.ofile) {
        fclose(status.ofile);
    }

    // errors_finalize();

    // if (errors_count() > 0) {
    //     return 1;
    // }

    return 0;
}