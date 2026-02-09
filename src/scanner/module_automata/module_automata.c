#include "module_automata.h"
#include <string.h>

static void set_int_list_end(int *arr, int cap) {
    for (int i = 0; i < cap; i++) arr[i] = -1;
}

bool automata_is_accepting(const AutomataDFA* a, int st) {
    for (int i = 0; i < MAX_STATES && a->accepting_states[i] != -1; i++) {
        if (a->accepting_states[i] == st) return true;
    }
    return false;
}

AutomataDFA automata_number_create(void) {
    AutomataDFA a;
    memset(&a, 0, sizeof(a));

    a.type = CAT_NUMBER;

    //Alphabet
    strcpy(a.alphabet, "0123456789");
    a.matrix.width = 10;

    //States: 1 init, 2 accepting, 3 sink
    set_int_list_end(a.states, MAX_STATES);
    a.states[0] = 1;
    a.states[1] = 2;
    a.states[2] = 3;
    a.matrix.height = 3;
    a.start_state = 1;
    a.current_state = a.start_state;
    set_int_list_end(a.accepting_states, MAX_STATES);
    a.accepting_states[0] = 2;

    // Lookahead acceptance: delimitadores típicos tras número
    strcpy(a.lookahead_acceptance, " \t\n;,.(){}[]+=*>");
    a.dont_look_anymore = false;

    for (int col = 0; col < a.matrix.width; col++) {
        a.matrix.states_rows[0].new_state[col] = 2; // from state 1
        a.matrix.states_rows[1].new_state[col] = 2; // from state 2
        a.matrix.states_rows[2].new_state[col] = 3; // from state 3
    }
    return a;
}