#include "./main.h"
#include "./module_parser/module_parser.h"

FILE* ofile = NULL; // The output handler for the project run

int main(int argc, char *argv[]) {
    errors_init();

    ofile = stdout; // Default output to stdout

    fprintf(ofile, "Starting module args ...\n");
    
    ArgFlags* flags = process_arguments(argc, argv);
    if (!flags) { //Something wesnt wrong since module_args did not return the flags
        return 1;
    }

    if (flags->show_help) { //Show help if requested
        show_help();
        free(flags);
        return 0;
    }

    fprintf(stdout, "Input file: %s\n", flags->ifile);
    fprintf(stdout, "Output file: %s\n", flags->ofile);
    fprintf(stdout, "Flags: remove_comments=%d, process_directives=%d\n",
            flags->remove_comments, flags->process_directives);

    ParserState* state = init_parser(flags->ifile, flags->ofile, flags);
    if (!state) {
        fprintf(stderr, "Error: Could not initialize parser\n");
        free(flags);
        return 1;
    }

    fprintf(stdout, "Preprocessing...\n");
    // Parse the input file until EOF
    // We use an empty array (only NULL) so parse_until will return -1 when it reaches actual EOF
    const char* no_stop[] = {NULL};  // Empty array means parse until actual EOF
    int result = parse_until(state, no_stop, true);

    fprintf(stdout, "Preprocessing completed!\n");
    fprintf(stdout, "Output written to: %s\n", flags->ofile);

    cleanup_parser(state);
    free(flags);

    errors_finalize();

    if (errors_count() > 0) {
        return 1;
    }

    return 0;
}