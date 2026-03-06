#ifndef MODULE_PARSER_H
#define MODULE_PARSER_H
#include "../config.h"

/* -----------------------------------------------------------------------------
 * module_parser.h
 *
 * Header for the parser engine (SRA) module.
 * Declares the entry points used by the parser driver and the SRA engine
 * implemented in `module_parser.c`.
 *
 * Responsibilities:
 *  - initialize and destroy AutomataSRA structures
 *  - query the ACTION/GOTO tables for next actions
 *  - perform reduce operations and stack updates
 *  - driver to run the SRA over a loaded token list
 *
 * Team: GA
 * Contributor/s: Andrea Sallo
 * -------------------------------------------------------------------------- */


void initialize_SRA_DFA_stack(LanguageV2 * language);

void destroy_language(LanguageV2* language);

ParseAction get_next_action(LanguageV2* language, Token token, RuleItemType* type);

void reduce_rule(Stack* stack, RuleV2* rule, Token *lhs, int *num_tokens, LanguageV2 * language);

ActionType update_automatasra(AutomataSRA *a, Token token, LanguageV2* language);

Token read_next_token(AutomataSRA* sra, int* returned);

int write_update_to_output(Stack stack, Token tokn, ParseAction op);

void automatasra_driver(LanguageV2 * language);

#endif // MODULE_PARSER_H
