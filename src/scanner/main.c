/*
 * -----------------------------------------------------------------------------
 * main.c
 *
 * Main entry point for the C preprocessor application. This program processes
 * C source files by handling preprocessor directives, macro definitions, and
 * conditional compilation.
 *
 * Program Flow:
 * 1. Initialize error tracking system
 * 2. Process command-line arguments (input/output files, flags)
 * 3. Initialize parser with input file and configuration
 * 4. Execute main parsing loop to process the entire input file
 * 5. Clean up resources and finalize error reporting
 * 6. Exit with appropriate status code based on error count
 *
 * Supported Features:
 * - Comment removal (-c flag)
 * - Directive processing (-d flag) including #include, #define, #ifdef, #ifndef
 * - Macro substitution
 * - Error tracking and reporting
 *
 * Usage:
 *     ./preprocessor <input_file> <output_file> [-c] [-d]
 *     Use -help flag for detailed usage information
 *
 * Exit Codes:
 *     0 - Success (no errors encountered)
 *     1 - Failure (errors occurred during preprocessing)
 *
 * Team: GA
 * Contributor/s: Gorka Hernández Villalón
 * -----------------------------------------------------------------------------
 */

#include "./main.h"

int main(int argc, char *argv[]) {
    errors_init();
    
    int returned = process_arguments(argc, argv);

    if (returned == HELP_RETURN) { //Show help if requested
        show_help();
        return 0;
    }

    fprintf(stdout, "Input file: %s\n", status.ifile);
    fprintf(stdout, "Output file: %s\n", status.ofile);
    
    // ParserState* state = init_parser(flags->ifile, flags->ofile, flags);
    // if (!state) {
    //     fprintf(stderr, "Error: Could not initialize parser\n");
    //     free(flags);
    //     return 1;
    // }

    fprintf(stdout, "Preprocessing...\n");
    // Parse the input file until EOF
    // We use an empty array (only NULL) so parse_until will return -1 when it reaches actual EOF
    const char* no_stop[] = {NULL};  // Empty array means parse until actual EOF
    int result = parse_until(no_stop, true);

    fprintf(stdout, "Preprocessing completed!\n");
    fprintf(stdout, "Output written to: %s\n", status.ofile);

    errors_finalize();

    if (errors_count() > 0) {
        return 1;
    }

    return 0;
}