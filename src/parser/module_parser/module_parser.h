#ifndef MODULE_PARSER_H
#define MODULE_PARSER_H
#include "../config.h"

AutomataSRA* initializeSRA(AutomataDFA* dfa, const ParseTable* table, ListTokens* tokens, Language* language);

#endif // MODULE_PARSER_H
