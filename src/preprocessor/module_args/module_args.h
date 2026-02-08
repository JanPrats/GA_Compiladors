/*
 * -----------------------------------------------------------------------------
 * module_args.h
 *
 * Header file for moduel_args, which provides functions to handle command-line
 * arguments in a modular and testable way.
 *
 * Functions:
 * - `print_arguments`: Logs each argument received by the program.
 * - `process_arguments`: Intended to handle application-specific argument logic.
 *                        It sets all flags from call to the preprocessor (CLI args) 
 *                          and sets the input file name and the output file name.
 * - `show_help`: Intended to show the manpage when the -help flag is called inline (CLI args)
 *
 * Usage:
 *     Include this header in main modules or test modules that require access
 *     to command-line processing functionality.
 *
 * Notes:
 *     This is part of a modular project structure, allowing each module to be
 *     developed and tested independently.
 *
 * Team: GA
 * Contributor/s: Pol García López
 * -----------------------------------------------------------------------------
 */

#ifndef MODULE2_H
#define MODULE2_H

#include "../main.h"
#include "../module_parser/module_parser.h"

typedef struct ArgFlags ArgFlags;   
ArgFlags* process_arguments(int argc, char *argv[]);
void show_help(void);
void print_arguments(int argc, char *argv[]);


#endif

