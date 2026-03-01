#ifndef MODULE_PARSER_H
#define MODULE_PARSER_H
#include "../config.h"

AutomataSRA* initializeSRA(AutomataDFA* dfa, const ParseTable* table, ListTokens* tokens, LanguageV2* language);

void destroySRA(AutomataSRA* sra);

ParseAction get_next_action(a, token, lookahead, &type_tnt);

void reduce_rule(Stack* stack, RuleV2* rule, Token *lhs, int *num_tokens);

ActionType update_automatasra(AutomataSRA *a, Token token, LanguageV2* language);

Token read_next_token(ListTokens * tokenlist, int* returned);

int write_update_to_output(Stack stack, Token tokn, ParseAction op);

void automatasra_driver(LanguageV2 * language);

#endif // MODULE_PARSER_H
