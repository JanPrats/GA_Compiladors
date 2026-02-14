#include "module_automata.h"
#include "../module_error/module_error.h"
#include <string.h>


int write_token_to_file_and_list(BufferAuto *buffer, Category cat){
    if (status.oform == RELEASE_M) {
        fprintf(status.ofile, "<%s, %s>\n", buffer->lexeme, category_to_string(cat));
    }
    if (status.oform == DEBUG_M) { //needs to be changed
        fprintf(status.ofile, "<%s, %s>\n", buffer->lexeme, category_to_string(cat));
    }
    add_token_to_list(buffer->lexeme, cat);

    return CORRECT_RETURN;

}


int search_column(AutomataDFA *a, char actual_character){
    for (int i = 0; i < MAX_ALPHABET_SIZE && a->alphabet[i].character != '\0'; i++){// Buscar l'índex del caràcter c dins l'alfabet de l'autòmata
        if (a->alphabet[i].character == actual_character){
            return a->alphabet[i].column; // Índex del símbol a la matriu
        }
    }
    return -1;
}

Two_ints search_two_columns(AutomataDFA *a, char actual_character, char lookahead){
    Two_ints result = { -1, -1 };   // default: not found
    for (int i = 0; i < MAX_ALPHABET_SIZE && a->alphabet[i].character != '\0'; i++){
        char symbol = a->alphabet[i].character;

        if (symbol == actual_character)
            result.actual = a->alphabet[i].column;

        if (symbol == lookahead)
            result.lookahead = a->alphabet[i].column;

        if (result.actual != -1 && result.lookahead != -1)
            break;  // both found → stop searching
    }
    return result;
}

bool is_accepting_state(AutomataDFA *a, int state){
    for (int i = 0; i < MAX_STATES && a->accepting_states[i] != 0; i++) {
        if (a->accepting_states[i] == state){
            return true;
        }
    }
    return false;
}

void restart_automatas(AutomataDFA **a, int num_automata){
    for (int i = 0; i < num_automata; i++){ // Reinicialitzar tots els autòmata per començar un nou token
        a[i]->current_state = a[i]->start_state;
        a[i]->dont_look_anymore = false;
    }
}

/**
 * Actualitza l'estat d'un autòmata amb el caràcter llegit
 * @param a Punter a l'autòmata DFA
 * @param c Caràcter a processar
 * @param lookahead_char Següent caràcter (per verificar lookahead)
 * @return Category del token si acceptat, 0 si rebutjat, -1 si continua processant
 */
static int update_automata(AutomataDFA *a, char c, char lookahead) {
    
    Two_ints acala = search_two_columns(a, c, lookahead); //Search the column of TransitionMatrix of both current character and lookahead character

    int new_state = a->matrix.states_rows[a->current_state].new_state[acala.actual]; 

    a->current_state = new_state; //Update Automata

    if (new_state == 0)             //This can only happen if the character is the first to be put from initial state. So we would already stop the automata,
        return STOP_AUTOMATA;       // In any other case we would have seen the Lookahead going to NULL state

    int lookahead_state = a->matrix.states_rows[new_state].new_state[acala.lookahead]; //See where we would go in the next step (with the lookahead)

    if (lookahead_state == 0){  //If in the next step we would go to NULL state we consider the TOKEN finished
        bool accept_token = is_accepting_state(a, new_state); 
        if (accept_token)       //If we are in an accepting state we accept the token
            return ACCEPT_TOKEN;
        return STOP_AUTOMATA;   //Else we would reject the TOKEN and stop the automata
    }
    return CORRECT_RETURN;
}

/**
 * Driver que processa un fitxer amb múltiples autòmata DFA
 * Escriu tokens reconeguts i no reconeguts en fitxers separats
 */
void automata_driver(AutomataDFA **automata_list, int num_automata){
    
    char c = fgetc(status.ifile); // Caràcter llegit del fitxer
    if (c == EOF){
        report_error_typed(ERR_EMPTY_FILE, 0);
        return;
    }
    
    char lookahead;
    BufferAuto buffer;
    BufferAuto buffer_nonrecognized;
    buffer_clear(&buffer);
    buffer_clear(&buffer_nonrecognized);
    
    Category nonrecognized_category = CAT_NONRECOGNIZED;

    while ((lookahead = fgetc(status.ifile)) != EOF){ //Anar llegint el fitxer
        bool accepted = false; // Indica si algun autòmata ha acceptat aquest token
        buffer_add(&buffer, c);

        for (int i = 0; i < num_automata; i++){// Iterar sobre tots els autòmata

            if (!automata_list[i]->dont_look_anymore){ // Només processar actius
                int decision = update_automata(automata_list[i], c, lookahead);

                if (decision == ACCEPT_TOKEN){ // L'autòmata ha arribat a un estat d'acceptació 
                    if(buffer_nonrecognized.len == 0){
                        write_token_to_file_and_list(&buffer_nonrecognized, nonrecognized_category);
                    }  
                    write_token_to_file_and_list(&buffer, automata_list[i]->type);
                    accepted = true; // Marcar que s'ha acceptat un token
                    break;           // Reiniciarem tots els autòmata després
                    
                } else if (decision == STOP_AUTOMATA){ // L'autòmata ha rebutjat, marcar com a "no mirar més"
                    automata_list[i]->dont_look_anymore = true; 
                }
                // value == -1 → continua processant, no fem res
            }
        }

        if (accepted){ //Si algun dels automates ha acceptat el token
            buffer_clear(&buffer);
            restart_automatas(automata_list, num_automata);
        }
        else {
            bool all_done = true;
            for (int i = 0; i < num_automata; i++){ // Comprovar si tots els autòmata han rebutjat el caràcter
                if (!automata_list[i]->dont_look_anymore){
                    all_done = false;
                    break;
                }
            }
            
            if (all_done){
                // fprintf(unrecognized_file, "<%c, %d>\n", c, CAT_NONRECOGNIZED); // Escriure el caràcter com a no reconegut en el fitxer corresponent
                buffer_move_append(&buffer_nonrecognized, &buffer);
                restart_automatas(automata_list, num_automata);
            }
        }
        c = lookahead; //lookahead becomes the actual char
    }
  
    if(c != EOF && status.all_tokens.count == 0){ //Case of file with only one character, we do not handle this case
        fprintf(status.ofile, "<%c, %s>\n",
                c, category_to_string(CAT_NONRECOGNIZED));
    }
}