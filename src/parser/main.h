/*
 * -----------------------------------------------------------------------------
 * main.h
 *
 * Header for the Parser main entry point.
 * Includes all modules needed by the parser:
 *   - config.h           : global status, data structures, constants
 *   - module_load_tokens : load token list from .cscn file
 *   - module_parser      : SRA initialization and driver
 *
 * Team: GA
 * Contributor/s: Gorka Hernández
 * -----------------------------------------------------------------------------
 */

#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "config.h"
#include "load_tokens/module_load_tokens.h"
#include "module_parser/module_parser.h"

#endif // MAIN_H
