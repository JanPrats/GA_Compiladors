//Jan Prats Soler - Module_comments_remove

#ifndef MODULE_COMMENTS_H
#define MODULE_COMMENTS_H

#include "../main.h"
#include <stdbool.h>

typedef struct ParserState ParserState;

int process_comment(ParserState* state, char current_char, char next_char, bool copy_to_output);

#endif

