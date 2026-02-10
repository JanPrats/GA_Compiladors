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

AutomataDFA return_automata = {
    .alphabet = { 'r', 'e', 't', 'u', 'n' },
    .states = { 0, 1, 2, 3, 4, 5, 6, 7 },
    .start_state = 1,
    .current_state = 1,
    .accepting_states = { 7 },
    .lookahead_acceptance = { ' ', ';', '\n', '\t', '(', '{' },
    .matrix = {
        .states_rows = {
            { .new_state = { 2, 0, 0, 0, 0 } }, 
            { .new_state = { 0, 3, 0, 0, 0 } },
            { .new_state = { 0, 0, 4, 0, 0 } },
            { .new_state = { 0, 0, 0, 5, 0 } },
            { .new_state = { 6, 0, 0, 0, 0 } },
            { .new_state = { 0, 0, 0, 0, 7 } },
            { .new_state = { 0, 0, 0, 0, 0 } },
            { .new_state = { 0, 0, 0, 0, 0 } }
        },
        .width = 5,   
        .height = 8   
    },
    .type = CAT_KEYWORD,
    .dont_look_anymore = false
};


AutomataDFA number_automata = {
    .alphabet = { '0','1','2','3','4','5','6','7','8','9' },
    .states = { 1, 2, 3 },        
    .start_state = 1,
    .current_state = 1,
    .accepting_states = { 2 },
    .lookahead_acceptance = {
        ' ', '\t', '\n',
        ';', ',', '.', 
        '(', ')', '{', '}', '[', ']',
        '+', '=', '*', '>'
    },

    .matrix = {
        .states_rows = {
            { .new_state = { 2,2,2,2,2,2,2,2,2,2 } },
            { .new_state = { 2,2,2,2,2,2,2,2,2,2 } },
            { .new_state = { 3,3,3,3,3,3,3,3,3,3 } }
        },
        .width  = 10,
        .height = 3
    },
    .type = CAT_NUMBER,
    .dont_look_anymore = false
};


AutomataDFA specials_automata = {
    .alphabet = { ';', ',', '(', ')', '{', '}', '[', ']'},
    .states = {0, 1, 2},
    .start_state = 0,
    .current_state = 0,
    .accepting_states = {1, 2},
    .lookahead_acceptance = { ' ', ';', '\n', '\t', '(', '{', '[', ']', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' },
    .matrix = {            
        .states_rows = {
            { .new_state = { 1, 1, 1, 1, 1, 1, 1, 1} }, 
            { .new_state = { 2, 2, 2, 2, 2, 2, 2, 2} }, 
            { .new_state = { 2, 2, 2, 2, 2, 2, 2, 2} }
        },
        .width = 8,   
        .height = 3  
    },
    .type = CAT_SPECIALCHAR,
    .dont_look_anymore = false
};

AutomataDFA operators_automata = {
    .alphabet = { '+', '-', '=', '!', '*', '/'},
    .states = {0, 1, 2, 3, 4, 5, 6},
    .start_state = 0,
    .current_state = 0,
    .accepting_states = {1, 2, 3, 4, 5, 6},
    .lookahead_acceptance = { ' ', ';', '\n', '\t', '(', '{', '[', ']', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' },
    .matrix = {            
        .states_rows = {
            { .new_state = { 2, 4, 1, 3} }, 
            { .new_state = { 6, 6, 5, 6} }, 
            { .new_state = { 5, 6, 6, 6} }, 
            { .new_state = { 6, 6, 6, 5} }, 
            { .new_state = { 6, 5, 6, 6} }, 
            { .new_state = { 6, 6, 6, 6} }, 
            { .new_state = { 6, 6, 6, 6} } 
        },
        .width = 4,   
        .height = 7  
    },
    .type = CAT_OPERATOR,
    .dont_look_anymore = false
};

/**
 * Actualitza l'estat d'un autòmata amb el caràcter llegit
 * @param a Punter a l'autòmata DFA
 * @param c Caràcter a processar
 * @param lookahead_char Següent caràcter (per verificar lookahead)
 * @return Category del token si acceptat, 0 si rebutjat, -1 si continua processant
 */
static int update_automata(AutomataDFA* a, char c, char lookahead_char) {
    // Buscar el caràcter c en el vocabulari de l'autòmata
    int count = 0;
    bool found = false;
    
    for (int i = 0; i < MAX_ALPHABET_SIZE && a->alphabet[i] != '\0'; i++) {
        if (a->alphabet[i] == c) {
            found = true;
            break;
        }
        count++;
    }
    
    // Si el caràcter no està en l'alfabet, l'autòmata rebutja
    if (!found) {
        return 0;
    }
    
    // Actualitzar l'estat actual amb la matriu de transició
    int new_state = a->matrix.states_rows[a->current_state].new_state[count];
    
    // Si new_state és 0, significa que no hi ha transició vàlida (estat de rebuig)
    if (new_state == 0) {
        return 0;
    }
    
    a->current_state = new_state;
    
    // Verificar si el lookahead està en lookahead_acceptance
    bool lookahead_valid = false;
    for (int i = 0; i < MAX_ALPHABET_SIZE && a->lookahead_acceptance[i] != '\0'; i++) {
        if (a->lookahead_acceptance[i] == lookahead_char) {
            lookahead_valid = true;
            break;
        }
    }
    
    // Si el lookahead és vàlid, verificar si estem en un estat d'acceptació
    if (lookahead_valid) {
        if (automata_is_accepting(a, a->current_state)) {
            return a->type; // Retorna la categoria del token
        } else {
            return 0; // No està en estat d'acceptació
        }
    }
    
    // Continuar processant
    return -1;
}

/**
 * Funció driver que processa un fitxer d'entrada amb múltiples autòmata
 * @param input_file Fitxer d'entrada
 * @param output_file Fitxer de sortida
 * @param automata_list Array d'autòmata
 * @param num_automata Nombre d'autòmata en la llista
 */
void automata_driver(FILE* input_file, FILE* output_file, AutomataDFA* automata_list, int num_automata) {
    char c;
    bool new = true;
    
    //INICI CODI QUE VAM FER AHIR -----------------------------------------------------------------------------------
    while ((c = fgetc(input_file)) != EOF) {
        // Si comencem un nou token, reinicialitzar tots els autòmata
        if (new) {
            for (int i = 0; i < num_automata; i++) {
                automata_list[i].dont_look_anymore = false;
                automata_list[i].current_state = automata_list[i].start_state;
            }
            new = false;
        }
        
        // Obtenir el següent caràcter per lookahead (sense consumir-lo)
        char lookahead_char = fgetc(input_file);
        if (lookahead_char != EOF) {
            ungetc(lookahead_char, input_file); // Retornar el caràcter al buffer
        }
        
        // Processar cada autòmata
        for (int i = 0; i < num_automata; i++) {
            if (!automata_list[i].dont_look_anymore) { // Si dont_look_anymore == true acabarem el bucle for 
                int value = update_automata(&automata_list[i], c, lookahead_char);
                
                if (value >= 0 && value != -1) { // És una Category vàlida
                    new = true;
                    fprintf(output_file, "<%c, %d>\n", c, value);
                } else if (value == 0) {
                    automata_list[i].dont_look_anymore = true;
                } else {
                    // value == -1, continuar processant
                    continue;
                }
            }
        }
        
        // Verificar si tots els autòmata han deixat de mirar
        bool all_done = true;
        for (int i = 0; i < num_automata; i++) {
            if (!automata_list[i].dont_look_anymore) {
                all_done = false;
                break;
            }
        }
        
        //FI DEL CODI QUE VAM FER AHIR -----------------------------------------------------------------------------------

        if (all_done) {
            // Cap autòmata ha reconegut el token
            Category not_identified = CAT_NONRECOGNIZED;
            new = true;
            fprintf(output_file, "<%c, %d>\n", c, not_identified);
        }
    }
}
