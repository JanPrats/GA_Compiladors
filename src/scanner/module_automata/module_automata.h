#ifndef MODULE_AUTOMATA_H
#define MODULE_AUTOMATA_H
#include <stdbool.h>
#include "config.h"

typedef struct StateRow {
    int new_state[MAX_STATES]; // Contains a state_number for each state-char pair S0_row = [1,0,3,0,2] (assume symbols from the alphabet are always in the same order)
} StateRow;
//???SHOULD BE [MAX_ALPHABET_SIZE] INSTEAD OF [MAX_STATES] ??????????????

typedef struct TransitionMatrix {
    StateRow states_rows[MAX_STATES]; // Contains a row for each state in the DFA [S0_row, S1_row, S2_row, ...]
    int width; //Number of symbols
    int height; //Number of states
} TransitionMatrix;

typedef struct AutomataDFA {
    char alphabet[MAX_ALPHABET_SIZE];              // [w,h,i,l,e] {if we need w for transition matrix it is 0, for h it is 1, etc; as they appear} //Could also be a dictionary
    int states[MAX_STATES];                         // [1,2,3,4,5,6,7] {could be just an int, but idk just in case}
    int start_state;                                // 1
    int current_state;                              // current_state (cs) changes when we read characters (cs == 1 and read w --> cs == 2)
    int accepting_states[MAX_STATES];               // [6] {could be more}
    char lookahead_acceptance[MAX_ALPHABET_SIZE];   // ["("," ","\n", EOF]
    TransitionMatrix matrix;                        // [[2,7,7,7,7],[7,3,7,7,7],[7,7,4,7,7],[7,7,7,5,7],[7,7,7,7,6],[7,7,7,7,7],[7,7,7,7,7]]
    Category type;                                  // CAT_KEYWORD (it is an enum, so include config.h to have the struct)
    bool dont_look_anymore;                         // If we have already finished the execution and we do not want to keep looking
} AutomataDFA;


AutomataDFA automata_number_create(void);

bool automata_is_accepting(const AutomataDFA* a, int st);

 #endif