/*
 * -----------------------------------------------------------------------------
 * module_comments_remove.h
 *
 * Header file for module_comments_remove, which provides functions to detect
 * and process C-style comments in source code.
 *
 * Functions:
 * - `process_comment`: Detects and processes both single-line (//) and 
 *                      multi-line comments.
 *                      Depending on parser configuration, comments can be:
 *                        - Removed entirely
 *                        - Preserved and copied to the output
 *
 * Usage:
 *     Include this header in parser modules or other components that need
 *     to handle comment detection and processing.
 *
 * Notes:
 *     Designed to support modular parsing, making comment handling
 *     configurable and testable.
 *
 * Team: JP
 * Contributor/s: Jan Prats Soler
 * -----------------------------------------------------------------------------
 */

#ifndef MODULE_COMMENTS_H
#define MODULE_COMMENTS_H

#include "../main.h"
#include <stdbool.h>

typedef struct ParserState ParserState;

int process_comment(ParserState* state, char current_char, char next_char, bool copy_to_output);

#endif

