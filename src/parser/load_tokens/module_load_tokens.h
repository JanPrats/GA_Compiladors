/*
 * -----------------------------------------------------------------------------
 * module_load_tokens.h
 *
 * Module to load/create the list of tokens in memory.
 * Supports two input sources:
 *   1. From a .cscn file (scanner output) - load_tokens_from_file()
 *   2. From the scanner directly (in-memory, already in status.all_tokens)
 *      via load_tokens_from_scanner()
 *
 * The loaded tokens are stored in status.all_tokens (ListTokens).
 *
 * Usage:
 *   - Call load_tokens_from_file(filepath) to parse a .cscn file
 *   - Call load_tokens_from_scanner() if the scanner already ran and status.all_tokens is already populated
 *
 * Team: GA
 * Contributor/s: Clara Serra Borràs
 * -----------------------------------------------------------------------------
 */

#ifndef MODULE_LOAD_TOKENS_H
#define MODULE_LOAD_TOKENS_H

#include "../config.h"


//   Loads the token list from a .cscn file into status.all_tokens.
//   Clears any existing tokens in status.all_tokens before loading.
//   The .cscn format is (per line):
//     RELEASE mode: <lexeme, CATEGORY> <lexeme, CATEGORY> ...
//     DEBUG mode:   <linenumber> <lexeme, CATEGORY> <lexeme, CATEGORY> ... (followed by blank line)
//   Returns:
//     CORRECT_RETURN on success
//     ERROR_RETURN   on failure (file not found, parse error, etc.)
 
int load_tokens_from_file(const char* filepath);


//  Uses the token list already in status.all_tokens (populated by the scanner). validates the list is non-empty.
//  Returns:
//    CORRECT_RETURN if tokens are present
//    ERROR_RETURN   if the list is empty

int load_tokens_from_scanner(void);

Category string_to_category(const char* str);

void clear_token_list(void);

#endif // MODULE_LOAD_TOKENS_H
