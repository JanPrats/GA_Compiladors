/*
 * -----------------------------------------------------------------------------
 * main.c
 *
 * Entry point for the Parser (Practice 3).
 *
 * Program Flow:
 *   1. Parse arguments: expect a .cscn file path
 *   2. Open output file  <stem>_p3dbg.txt
 *   3. Load tokens from the .cscn file into status.all_tokens
 *   4. (TODO) Load language from language.txt into a Language struct
 *   5. (TODO) Initialize SRA and run the parser driver
 *   6. Close output file
 *
 * Usage:
 *   parser <input_file.cscn>
 *   parser -help
 *
 * Exit Codes:
 *   0 - Success
 *   1 - Failure (missing argument, file not found, token load error)
 *
 * Team: GA
 * Contributor/s:
 * -----------------------------------------------------------------------------
 */

#include "main.h"

/* -----------------------------------------------------------------------
 * show_help
 * Prints usage information to stdout.
 * ----------------------------------------------------------------------- */
static void show_help(void) {
    printf("Usage: parser <input_file.cscn>\n");
    printf("Options:\n");
    printf("  -help    Display this help message\n");
    printf("\n");
    printf("Output: <input_file>_p3dbg.txt\n");
}

/* -----------------------------------------------------------------------
 * build_output_filename
 * Builds the output filename from the input filename.
 * Strips the extension from `input` and appends "_p3dbg.txt".
 * Result is written into `out` (size `out_size`).
 * ----------------------------------------------------------------------- */
static void build_output_filename(const char *input, char *out, size_t out_size) {
    char stem[MAX_FILENAME];
    strncpy(stem, input, MAX_FILENAME - 1);
    stem[MAX_FILENAME - 1] = '\0';

    char *dot = strrchr(stem, '.');
    if (dot) *dot = '\0';   /* remove extension */

    snprintf(out, out_size, "%s_p3dbg.txt", stem);
}

/* -----------------------------------------------------------------------
 * init_parser
 * Stores the input filename in status and opens the output file.
 * Returns CORRECT_RETURN on success, ERROR_RETURN on failure.
 * ----------------------------------------------------------------------- */
static int init_parser(const char *input_file) {
    strncpy(status.ifile_name, input_file, MAX_FILENAME - 1);
    status.ifile_name[MAX_FILENAME - 1] = '\0';

    build_output_filename(status.ifile_name, status.ofile_name, MAX_FILENAME);

    status.ofile = fopen(status.ofile_name, "w");
    if (!status.ofile) {
        fprintf(stderr, "Error: could not open output file '%s'\n", status.ofile_name);
        return ERROR_RETURN;
    }
    return CORRECT_RETURN;
}

/* -----------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int main(int argc, char *argv[]) {

    /* --- Argument check ------------------------------------------------ */
    if (argc < 2) {
        show_help();
        return 1;
    }

    if (strcmp(argv[1], HELP_F) == 0) {
        show_help();
        return 0;
    }

    /* --- Initialise: open output file ---------------------------------- */
    if (init_parser(argv[1]) != CORRECT_RETURN) {
        return 1;
    }

    /* --- Detectar extensió i carregar tokens -------------------------------- */
    char path[MAX_FILENAME], filename[MAX_FILENAME], extension[MAXFILEEXT];
    split_path(status.ifile_name, path, filename, extension);

    if (strcmp(extension, "cscn") == 0) {
        // Entrada directa: fitxer de tokens del scanner
        int ret = load_tokens_from_file(status.ifile_name);
        if (ret != CORRECT_RETURN) {
            fprintf(stderr, "Error: failed to load tokens from '%s'\n", status.ifile_name);
            if (status.ifile)  { fclose(status.ifile);  status.ifile  = NULL; }
            if (status.ofile)  { fclose(status.ofile);  status.ofile  = NULL; }
            return 1;
        }
    } else if (strcmp(extension, "c") == 0) {
        // Entrada .c: primer executem el scanner, després el parser llegeix els tokens de memòria
        AutomataList automata_list;
        init_automata(&automata_list);
        automata_driver(automata_list.automatas, automata_list.num_automata);
        // status.all_tokens ja està ple perquè el scanner usa add_token_to_list()
    } else {
        fprintf(stderr, "Error: unsupported file extension '.%s'. Use .cscn or .c\n", extension);
        return 1;
    }

    fprintf(status.ofile, "[INFO] Loaded %d tokens from '%s'\n",
            status.all_tokens.count, status.ifile_name);

    /*
     * // TODO (module_parser): Load language definition and run SRA.
     *
     * Language language;
     * load_language("language.txt", &language);
     *
     * AutomataDFA  dfa;
     * ParseTable   table;
     * build_parse_table(&language, &dfa, &table);
     *
     * AutomataSRA *sra = initializeSRA(&dfa, &table, &status.all_tokens, &language);
     * automatasra_driver(sra);
     * destroySRA(sra);
     */

    /* --- Close output file --------------------------------------------- */
    if (status.ofile) {
        fclose(status.ofile);
        status.ofile = NULL;
    }

    return 0;
}
