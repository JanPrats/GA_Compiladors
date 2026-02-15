/*
 * -----------------------------------------------------------------------------
 * main.h
 *
 * Main header file for the Scanner module.
 * Declares core scanning functions, includes necessary dependencies,
 * and provides function prototypes for lexical analysis.
 *
 * Key Functions:
 * - scan_token(): Primary scanning function that recognizes tokens
 *                using finite automata matching
 *
 * Function Behavior:
 * - Dispatches to appropriate DFA based on current character
 * - Recognizes all token categories (numbers, identifiers, keywords, etc.)
 * - Returns CAT_NONRECOGNIZED for unmatched input
 * - Advances input position past consumed characters
 *
 * Team: GA
 * Contributor/s:
 * -----------------------------------------------------------------------------
 */

#ifndef MAIN_H
#define MAIN_H
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "config.h"
#include "module_automata/module_automata.h"
#include "module_error/module_error.h"
#include "module_init/module_init.h"
// Jan por hacer: Single token scan: runs all automata on the input starting at input[*idx].
// Dispatches to appropriate DFA(s) based on current character.
// Returns the recognized token and advances *idx past consumed characters.
// If no automaton recognizes the input, returns a CAT_NONRECOGNIZED token.
Token scan_token(const char* input, int* idx, int line,AutomataDFA* automata_list, int automata_count);
#endif // MAIN_H
