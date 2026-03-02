/*
 * -----------------------------------------------------------------------------
 * module_load_tokens.c
 *
 * Token loader module for the Parser.
 * Reads a .cscn file (output of the scanner) and loads all tokens into
 * status.all_tokens so the parser can consume them.
 *
 * .cscn format:
 *   Each token is written as: <lexeme, CATEGORY>
 *   Tokens are separated by spaces or newlines.
 *   Example:
 *     <if, CAT_KEYWORD> <(, CAT_SPECIALCHAR> <x, CAT_IDENTIFIER>
 *     <3, CAT_NUMBER> <+, CAT_OPERATOR>
 *
 * Generic operations (add_token_to_list, category_to_string, etc.) are in
 * config.c and are reused here directly.
 *
 * Team: GA
 * Contributor/s: Clara Serra Borràs
 * -----------------------------------------------------------------------------
 */

#include "../main.h"
#include "../config.h"
#include "module_load_tokens.h"

/* -------------------------------------------------------------------------
 * Reads characters from f into buf (max len-1 chars) until 'stop' is found.
 * ------------------------------------------------------------------------- */
static int read_field(FILE* f, char* buf, int len, char stop) {
    int i = 0;
    int c;

    while ((c = fgetc(f)) != EOF) {
        if ((char)c == stop) break;
        if (i < len - 1) buf[i++] = (char)c;
    }
    buf[i] = '\0';

    if (c == EOF && i == 0) return -1;

    // Trim trailing spaces
    while (i > 0 && buf[i - 1] == ' ') buf[--i] = '\0';

    // Trim leading spaces
    int start = 0;
    while (buf[start] == ' ') start++;
    if (start > 0) memmove(buf, buf + start, i - start + 1);

    return i - start;
}

/* -------------------------------------------------------------------------
 * Opens the .cscn file, parses every <lexeme, CATEGORY> entry and adds each token to status.all_tokens via add_token_to_list() from config.c.
 * ------------------------------------------------------------------------- */
int load_tokens_from_file(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "File not found: %s\n", filename);
        return ERROR_RETURN;
    }
    // Reset token list before loading
    status.all_tokens.count = 0;
    char lexeme[MAX_TOKEN_NAME];
    char cat_str[MAX_TOKEN_NAME];

    int c;
    while ((c = fgetc(f)) != EOF) {
        if ((char)c != '<') continue;   // advance until next token 
        // Read lexeme
        if (read_field(f, lexeme, MAX_TOKEN_NAME, ',') < 0) break;
        // Read category
        if (read_field(f, cat_str, MAX_TOKEN_NAME, '>') < 0) break;
        Category cat = string_to_category(cat_str);
        add_token_to_list(lexeme, cat);  // defined in config.c (de l'altra pràctica)
    }

    fclose(f);
    return CORRECT_RETURN;
}

/* -------------------------------------------------------------------------
 * temporal sets up the scanner's required state (ifile, line counters, ofile) to run automata_driver() over the .c file
 * will write the scanner's output to /dev/null to avoid polluting the parser's output file, but the important side-effect is that add_token_to_list() fills status.all_tokens.
 * ------------------------------------------------------------------------- */
static int load_tokens_from_scan(void) {
    // Open the .c source file for the scanner to read
    status.ifile = fopen(status.ifile_name, "r");
    if (!status.ifile) {
        fprintf(stderr, "File not found: %s\n", status.ifile_name);
        return ERROR_RETURN;
    }

    FILE* scanner_out = fopen("/dev/null", "w");
    if (!scanner_out) scanner_out = status.ofile; 

    FILE* saved_ofile = status.ofile;
    status.ofile = scanner_out;

    // Reset scanner-specific status fields
    status.line = 1;
    status.first_token_in_line = true;
    status.line_has_tokens = false;
    status.all_tokens.count = 0;

    // Run the scanner
    AutomataList automata_list;
    init_automata(&automata_list);
    automata_driver(automata_list.automatas, automata_list.num_automata);

    // Restore parser state
    fclose(status.ifile);
    status.ifile = NULL;
    if (scanner_out != saved_ofile) fclose(scanner_out);
    status.ofile = saved_ofile;
    return CORRECT_RETURN;
}

/* -------------------------------------------------------------------------
 * Detects the file extension
 *   .cscn  -> load_tokens_from_file()   (parse scanner output file)
 *   .c     -> load_tokens_from_scan()   (run scanner in-memory)
 * ------------------------------------------------------------------------- */
int load_tokens(void) {
    char path[MAX_FILENAME], filename[MAX_FILENAME], extension[MAXFILEEXT];
    split_path(status.ifile_name, path, filename, extension);

    if (strcmp(extension, "cscn") == 0) {
        return load_tokens_from_file(status.ifile_name);

    } else if (strcmp(extension, "c") == 0) { //scanner first
        return load_tokens_from_scan();

    } else {
        fprintf(stderr, "Error: unsupported file extension '.%s'. Use .cscn or .c\n", extension);
        return ERROR_RETURN;
    }
}