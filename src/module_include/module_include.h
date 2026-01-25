#ifndef MODULE_INCLUDE_H
#define MODULE_INCLUDE_H

#include "../main.h"
#include <stdbool.h>

typedef struct ParserState ParserState;

int process_include(ParserState* state, bool copy_to_output);

void module_include_run(void);

#endif
