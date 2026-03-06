#ifndef MODULE_PARSER_H
#define MODULE_PARSER_H
#include "../config.h"

void initialize_SRA_DFA_stack(LanguageV2 * language);

void destroy_language(LanguageV2* language);

ParseAction get_next_action(LanguageV2* language, Token token, RuleItemType* type);

void reduce_rule(Stack* stack, RuleV2* rule, Token *lhs, int *num_tokens, LanguageV2 * language);

ActionType update_automatasra(AutomataSRA *a, Token token, LanguageV2* language);

Token read_next_token(AutomataSRA* sra, int* returned);

int write_update_to_output(Stack stack, Token tokn, ParseAction op);

void automatasra_driver(LanguageV2 * language);

#endif // MODULE_PARSER_H
