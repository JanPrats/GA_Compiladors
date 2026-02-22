/*
 * -----------------------------------------------------------------------------
 * main.c
 *
 *
 * Program Flow:
 *
 * Usage:
 *
 * Exit Codes:
 *     0 - Success (scanning completed, may have non-critical errors)
 *     1 - Failure (critical errors occurred during scanning)
 *
 * Team: GA
 * Contributor/s: Gorka Hernandez, Andrea Salló
 * -----------------------------------------------------------------------------
 */

#include "./main.h"
#include "config.h"
#include "count.h"
#include "module_init/module_init.h"

int main(int argc, char *argv[]) {
    error_init();
    
    // Initialize operation counting system
    
    int returned = init_program(argc, argv);
    

    if (returned == HELP_RETURN) {
        return 0;
    }

    if (returned != CORRECT_RETURN) {
        report_error_typed(ERR_INVALID_ARGUMENT, 0, SCANNER_STEP);
        error_finalize();
        return 1;
    }

    // Initialize automata for token recognition
    AutomataList automata_list;
    init_automata(&automata_list);

    // Run automata driver to scan input and generate tokens
    automata_driver(automata_list.automatas, automata_list.num_automata);

    // Finalize errors BEFORE closing files (error_finalize may write to ofile in debug mode)
    error_finalize();

    // Finalize counting system BEFORE closing files
    COUNT_FINALIZE();

    // Close files
    if (status.ifile) {
        fclose(status.ifile);
        status.ifile = NULL;
    }
    if (status.ofile) {
        fclose(status.ofile);
        status.ofile = NULL;
    }


    return (error_count() > 0) ? 1 : 0;
}
