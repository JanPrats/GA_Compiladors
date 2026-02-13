#ifndef MODULE_AUTOMATA_H
#define MODULE_AUTOMATA_H
#include <stdbool.h>
#include "../config.h"






int write_token_to_file_and_list(BufferAuto *buffer, Category cat);

int search_column(AutomataDFA *a, char actual_character);

Two_ints search_two_columns(AutomataDFA *a, char actual_character, char lookahead);

bool is_accepting_state(AutomataDFA *a, int state);

void restart_automatas(AutomataDFA **a, int num_automata);

/**
 * Driver que processa un fitxer amb múltiples autòmata DFA
 * Escriu tokens reconeguts i no reconeguts en fitxers separats
 */
void automata_driver(AutomataDFA **automata_list, int num_automata);

 #endif
