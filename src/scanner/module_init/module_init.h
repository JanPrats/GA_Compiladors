/*
 * -----------------------------------------------------------------------------
 * module_init.h
 *
 * Provides function declarations for scanner setup and startup.
 * Handles argument processing, status initialization, and automata creation.
 *
 * Key Functions:
 * - init_program(): Parse arguments and validate input
 * - init_status_scn(): Initialize scanner status structure
 * - init_automata(): Create and initialize all DFAs
 * - show_help(): Display usage information
 *
 * Automata Creation:
 * Each automata type (numbers, keywords, identifiers, etc.) is
 * initialized through dedicated create functions.
 *
 * Team: GA
 * Contributor/s: Pol García
 * -----------------------------------------------------------------------------
 */

#ifndef MODULE_INIT_H
#define MODULE_INIT_H
#include "../config.h"
#include "../module_automata/module_automata.h"
// Pol hacer init_automata
// Creates DFAs for each token category (numbers, keywords, types, identifiers,
// operators, special chars, literals, non-recognized) and stores them in automata_list.
// Sets *count to the number of automata created.
// Each DFA is created by its respective create function:
//   - automata_number_create()         
//   - automata_keyword_create()        
//   - automata_type_create()           
//   - automata_identifier_create()     
//   - automata_operator_create()       
//   - automata_special_create()        
//   - automata_literal_create()        
//   - automata_nonrecognized_create()  

int init_status_scn(void);
void init_automata(AutomataList* automata_list);
int init_program(int argc, char* argv[]);
void show_help(void);
#endif // MODULE_INIT_H