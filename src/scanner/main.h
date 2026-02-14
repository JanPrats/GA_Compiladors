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
