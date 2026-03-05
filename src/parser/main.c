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
    init_status_prs();
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
        status.help = true;
        show_help();
        return 0;
    }

    /* --- Initialise: open output file ---------------------------------- */
    if (init_parser(argv[1]) != CORRECT_RETURN) {
        return 1;
    }
    /* --- Carregar tokens (detecta extensió i delega al mòdul) -------------- */
    if (load_tokens() != CORRECT_RETURN) {
        if (status.ofile) { fclose(status.ofile); status.ofile = NULL; }
        return 1;
    }

    fprintf(status.ofile, "[INFO] Loaded %d tokens from '%s'\n",
            status.all_tokens.count, status.ifile_name);

    /* --- Load language definition from language.txt -------------------- */
    LanguageV2 language;
    memset(&language, 0, sizeof(language));

    if (load_language("language.txt", &language) != CORRECT_RETURN) { //To be done
        fprintf(stderr, "Error: could not load language from 'language.txt'\n");
        if (status.ofile) { fclose(status.ofile); status.ofile = NULL; }
        return 1;
    }

    fprintf(status.ofile, "[INFO] Language loaded: %d terminals, %d productions\n",
            language.num_terminals, language.num_productions);

    /* --- Build parse table (action + goto) from loaded language -------- */
    AutomataDFA  dfa;
    ParseTable   table;
    memset(&dfa, 0, sizeof(dfa));
    memset(&table, 0, sizeof(table));

    if (build_parse_table(&language, &dfa, &table) != CORRECT_RETURN) { //To be done
        fprintf(stderr, "Error: could not build parse table\n");
        if (status.ofile) { fclose(status.ofile); status.ofile = NULL; }
        return 1;
    }

    /* --- Initialize SRA and run parser driver -------------------------- */
    AutomataSRA *sra = initializeSRA(&dfa, &table);
    if (!sra) {
        fprintf(stderr, "Error: could not initialize SRA\n");
        if (status.ofile) { fclose(status.ofile); status.ofile = NULL; }
        return 1;
    }

    language.sra = sra;
    automatasra_driver(&language);
    destroySRA(sra);

    /* --- Close output file --------------------------------------------- */
    if (status.ofile) {
        fclose(status.ofile);
        status.ofile = NULL;
    }

    return 0;
}
