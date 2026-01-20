#ifndef MODULE_MACROS_H
#define MODULE_MACROS_H

#include "../main.h"
#include <stdbool.h>

// Parser initialization and cleanup
ParserState* init_parser(const char* input_file, const char* output_file, PreprocessorFlags* flags);
void cleanup_parser(ParserState* state);

// Main parsing function
int parse_until(ParserState* state, const char* stop_symbol, bool copy_to_output);

// Helper functions for character/word reading
char read_char(ParserState* state);
char peek_char(ParserState* state);


#endif

