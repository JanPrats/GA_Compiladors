#ifndef MODULE_PARSER_H
#define MODULE_PARSER_H
#include "../config.h"

AutomataSRA* initializeSRA(AutomataDFA* dfa, const ParseTable* table, ListTokens* tokens, Language* language);

void destroySRA(AutomataSRA* sra);

ActionType update_automata(AutomataSRA *a, Token tokn, Token lookahead);

Token read_next_token(ListTokens * tokenlist, int* returned);

int write_update_to_output(Stack stack, Token tokn, ActionType op);

void automata_driver(AutomataSRA *automata_sra);

#endif // MODULE_PARSER_H
