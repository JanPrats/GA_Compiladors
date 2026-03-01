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
 * Trims leading and trailing spaces from the result.
 * Returns number of chars read, or -1 on EOF with nothing read.
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
 * Opens the .cscn file, parses every <lexeme, CATEGORY> entry and adds
 * each token to status.all_tokens via add_token_to_list() from config.c.
 *
 * Returns CORRECT_RETURN (0) on success, ERROR_RETURN (-1) on failure.
 * ------------------------------------------------------------------------- */
int load_tokens_from_file(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        report_error_typed(ERR_FILE_NOT_FOUND, 0, PARSER_STEP);
        return ERROR_RETURN;
    }
    // Reset token list before loading
    status.all_tokens.count = 0;
    char lexeme[MAX_TOKEN_NAME];
    char cat_str[MAX_TOKEN_NAME];

    int c;
    while ((c = fgetc(f)) != EOF) {
        if ((char)c != '<') continue;   // advance until next token starts

        // Read lexeme: everything up to ','
        if (read_field(f, lexeme, MAX_TOKEN_NAME, ',') < 0) break;

        // Read category: everything up to '>'
        if (read_field(f, cat_str, MAX_TOKEN_NAME, '>') < 0) break;

        Category cat = string_to_category(cat_str);
        add_token_to_list(lexeme, cat);  // defined in config.c
    }

    fclose(f);
    return CORRECT_RETURN;
}