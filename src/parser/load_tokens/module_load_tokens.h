/*
 * -----------------------------------------------------------------------------
 * module_load_tokens.h
 *
 * Token loader module for the Parser.
 * Reads a .cscn file and populates status.all_tokens.
 *
 * .cscn format:
 *   <lexeme, CATEGORY> <lexeme, CATEGORY> ...
 *   Tokens can span multiple lines. Each token is enclosed in < >.
 *   Example:
 *     <if, CAT_KEYWORD> <(, CAT_SPECIALCHAR> <x, CAT_IDENTIFIER>
 *     <3, CAT_NUMBER> <+, CAT_OPERATOR>
 *
 * Generic token/list operations (add_token_to_list, etc.) live in config.c.
 * This module only handles the file parsing logic.
 *
 * Team: GA
 * Contributor/s: Clara Serra Borràs
 * -----------------------------------------------------------------------------
 */

#ifndef MODULE_LOAD_TOKENS_H
#define MODULE_LOAD_TOKENS_H

#include "../config.h"

static int read_field(FILE* f, char* buf, int len, char stop);
int load_tokens_from_file(const char* filename);

#endif // MODULE_LOAD_TOKENS_H