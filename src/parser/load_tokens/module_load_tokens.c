/*
 * -----------------------------------------------------------------------------
 * module_load_tokens.c
 *
 * Implementation of token list loading for the parser.
 * Parses .cscn files (scanner output) and status.all_tokens.
 *
 * .cscn token format per entry: <lexeme, CATEGORY>
 *   - Tokens are space-separated on a line
 *   - RELEASE mode: one group of tokens per source line, no blank lines
 *   - DEBUG mode:   line starts with a line number, blank line after each group
 *
 * Both modes are handled transparently by detecting if the first token on a
 * line starts with '<' (RELEASE) or is a number (DEBUG).
 *
 * Team: GA
 * Contributor/s:  Clara Serra Borràs
 * -----------------------------------------------------------------------------
 */

#include "module_load_tokens.h"
#include "../module_error/module_error.h"
#include <string.h>
#include <ctype.h>

/*
 * Converts a category string back to the Category enum.
 */
Category string_to_category(const char* str) {
    if (strcmp(str, "NUMBER")       == 0) return CAT_NUMBER;
    if (strcmp(str, "IDENTIFIER")   == 0) return CAT_IDENTIFIER;
    if (strcmp(str, "KEYWORD")      == 0) return CAT_KEYWORD;
    if (strcmp(str, "TYPE")         == 0) return CAT_TYPE;
    if (strcmp(str, "LITERAL")      == 0) return CAT_LITERAL;
    if (strcmp(str, "OPERATOR")     == 0) return CAT_OPERATOR;
    if (strcmp(str, "SPECIALCHAR")  == 0) return CAT_SPECIALCHAR;
    if (strcmp(str, "NONRECOGNIZED")== 0) return CAT_NONRECOGNIZED;
    return CAT_NONRECOGNIZED;
}

void clear_token_list(void) {
    status.all_tokens.count = 0;
}


static const char* parse_one_token(const char* src, char* out_lexeme, int lex_max, Category* out_cat) {
    if (*src != '<') return NULL;
    src++; // skip '<'

    // Find the closing '>'
    const char* close = strchr(src, '>');
    if (!close) return NULL;

    // Copy the interior: "lexeme, CATEGORY"
    int inner_len = (int)(close - src);
    if (inner_len <= 0 || inner_len >= MAX_LINE_LENGTH) return NULL;

    char inner[MAX_LINE_LENGTH];
    strncpy(inner, src, inner_len);
    inner[inner_len] = '\0';

    // Find the LAST ", " to split lexeme from category
    // (lexeme itself could contain commas, but category won't have ", " inside)
    char* last_sep = NULL;
    char* search = inner;
    while ((search = strstr(search, ", ")) != NULL) {
        last_sep = search;
        search += 2;
    }
    if (!last_sep) return NULL;

    // lexeme is inner[0 .. last_sep-inner)
    int lex_len = (int)(last_sep - inner);
    if (lex_len < 0 || lex_len >= lex_max) return NULL;
    strncpy(out_lexeme, inner, lex_len);
    out_lexeme[lex_len] = '\0';

    // category string starts after ", "
    char* cat_str = last_sep + 2;
    // trim trailing whitespace (just in case)
    int cat_len = (int)strlen(cat_str);
    while (cat_len > 0 && isspace((unsigned char)cat_str[cat_len-1])) {
        cat_str[--cat_len] = '\0';
    }

    *out_cat = string_to_category(cat_str);

    return close + 1; // pointer just after '>'
}


static void parse_line_tokens(const char* line, int line_number) {
    const char* p = line;

    // Skip leading whitespace
    while (*p && isspace((unsigned char)*p)) p++;

    // DEBUG mode: line may start with a line number (digits) before first '<'
    // If the first non-space char is a digit, read and skip the number.
    // That number is the original source line number — use it for the token.
    if (*p && isdigit((unsigned char)*p)) {
        int src_line = 0;
        while (*p && isdigit((unsigned char)*p)) {
            src_line = src_line * 10 + (*p - '0');
            p++;
        }
        // override line_number with the one embedded in the file
        line_number = src_line;
        // skip space(s) after the number
        while (*p && isspace((unsigned char)*p)) p++;
    }

    // Now parse all <lexeme, CATEGORY> entries on this line
    while (*p) {
        if (*p != '<') {
            p++; // skip unexpected character
            continue;
        }

        char lexeme[MAX_TOKEN_NAME];
        Category cat;
        const char* next = parse_one_token(p, lexeme, MAX_TOKEN_NAME, &cat);
        if (!next) {
            // Malformed entry — skip to next '<'
            p++;
            continue;
        }

        // Add token to the list
        if (status.all_tokens.count < MAX_TOKENS) {
            Token* t = &status.all_tokens.tokens[status.all_tokens.count];
            strncpy(t->lexeme, lexeme, MAX_TOKEN_NAME - 1);
            t->lexeme[MAX_TOKEN_NAME - 1] = '\0';
            t->cat  = cat;
            t->line = line_number;
            status.all_tokens.count++;
        } else {
            report_warning("Maximum token count reached while loading file, token discarded",
                           line_number, PARSER_STEP);
        }

        p = next;
        // skip whitespace between tokens
        while (*p && isspace((unsigned char)*p) && *p != '\n') p++;
    }
}


int load_tokens_from_file(const char* filepath) {
    if (!filepath) {
        report_error_typed(ERR_INVALID_ARGUMENT, 0, PARSER_STEP);
        return ERROR_RETURN;
    }

    FILE* f = fopen(filepath, "r");
    if (!f) {
        report_error_typed(ERR_FILE_NOT_FOUND, 0, PARSER_STEP);
        return ERROR_RETURN;
    }

    // Clear existing tokens
    clear_token_list();

    char line[MAX_LINE_LENGTH];
    int  file_line = 0; // line counter inside the .cscn file

    while (fgets(line, sizeof(line), f)) {
        file_line++;

        // Remove trailing newline/carriage return
        int len = (int)strlen(line);
        while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r')) {
            line[--len] = '\0';
        }

        // Skip blank lines (DEBUG mode inserts them between token groups)
        if (len == 0) continue;

        parse_line_tokens(line, file_line);
    }

    fclose(f);
    return CORRECT_RETURN;
}

int load_tokens_from_scanner(void) {
    if (status.all_tokens.count == 0) {
        report_warning("load_tokens_from_scanner: token list is empty", 0, PARSER_STEP);
        return ERROR_RETURN;
    }
    // Tokens already in status.all_tokens — nothing to do.
    return CORRECT_RETURN;
}
