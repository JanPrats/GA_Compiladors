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
void init_automata(AutomataDFA* automata_list, int* count);
#endif // MODULE_INIT_H