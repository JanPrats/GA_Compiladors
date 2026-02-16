/*
 * -----------------------------------------------------------------------------
 * module_automata.c
 *
 * Automata execution engine for the Scanner module.
 * Implements DFA (Deterministic Finite Automata) operations including:
 * state transitions, acceptance state checking, and token recognition.
 *
 * Key Functions:
 * - search_column(): Maps input character to automata alphabet column
 * - search_two_columns(): Maps both current and lookahead characters
 * - is_accepting_state(): Checks if a state is an accepting state
 * - restart_automatas(): Resets all automata to initial state
 * - write_token_to_file_and_list(): Records recognized token
 * - automata_driver(): Main driver for scanning entire input
 *
 * Features:
 * - Multi-automata support with cooperative matching
 * - Lookahead capability for complex token recognition
 * - Token buffering and output generation
 * - Line tracking for error reporting
 *
 * Team: GA
 * Contributor/s: Pol García, Clara Serra, Jan Prats
 * -----------------------------------------------------------------------------
 */

#include "module_automata.h"
#include "../module_error/module_error.h"
#include "../count.h"
#include <string.h>


int write_token_to_file_and_list(BufferAuto *buffer, Category cat){
    COUNT_COMP(1);
    if (status.oform == DEBUG && status.first_token_in_line) { //escriure numero linea al debug
        fprintf(status.ofile, "%d ", status.line);
        status.first_token_in_line = false;
        COUNT_IO(1);
        COUNT_GEN(1);
    }
    fprintf(status.ofile, "<%s, %s> ", buffer->lexeme, category_to_string(cat));
    COUNT_IO(1);

    status.line_has_tokens = true;
    add_token_to_list(buffer->lexeme, cat);
    COUNT_GEN(1);

    return CORRECT_RETURN;

}


int search_column(AutomataDFA *a, char actual_character){
    COUNT_GEN(1);  // for loop initialization
    for (int i = 0; i < MAX_ALPHABET_SIZE && a->alphabet[i].character != '\0'; i++){// Buscar l'índex del caràcter c dins l'alfabet de l'autòmata
        COUNT_COMP(2);  // loop condition (2 comparisons)
        COUNT_COMP(1);  // if condition
        if (a->alphabet[i].character == actual_character){
            COUNT_GEN(1);  // return statement
            return a->alphabet[i].column; // Índex del símbol a la matriu
        }
        COUNT_GEN(1);  // loop increment
    }
    COUNT_GEN(1);  // return statement
    return -1;
}

Two_ints search_two_columns(AutomataDFA *a, char actual_character, char lookahead){
    int count = 0;
    COUNT_GEN(2);
    Two_ints result = { -1, -1 };   // default: not found
    for (int i = 0; i < MAX_ALPHABET_SIZE && a->alphabet[i].character != EOF; i++){
        char symbol = a->alphabet[i].character;
        count++;

        if (symbol == actual_character)
            result.actual = a->alphabet[i].column;

        if (symbol == lookahead)
            result.lookahead = a->alphabet[i].column;

        if (result.actual != -1 && result.lookahead != -1){
            COUNT_COMP(4*count);
            COUNT_GEN(2*count);
            break;  // both found → stop searching
        }
    }
    COUNT_COMP(4*count);
    COUNT_GEN(2*count);
    return result;
}

bool is_accepting_state(AutomataDFA *a, int state){
    COUNT_GEN(1);  // for loop initialization
    for (int i = 0; i < MAX_STATES && a->accepting_states[i] != 0; i++) {
        COUNT_COMP(2);  // loop condition (2 comparisons)
        COUNT_COMP(1);  // if condition
        if (a->accepting_states[i] == state){
            COUNT_GEN(1);  // return statement
            return true;
        }
        COUNT_GEN(1);  // loop increment
    }
    COUNT_GEN(1);  // return statement
    return false;
}

void restart_automatas(AutomataDFA **a, int num_automata){
    COUNT_GEN(2*num_automata);
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

    COUNT_GEN(2);
    
    COUNT_COMP(1);
    if (new_state == 0)             //This can only happen if the character is the first to be put from initial state. So we would already stop the automata,
        return STOP_AUTOMATA;       // In any other case we would have seen the Lookahead going to NULL state

    int lookahead_state = a->matrix.states_rows[new_state].new_state[acala.lookahead]; //See where we would go in the next step (with the lookahead)

    COUNT_GEN(1);
    COUNT_COMP(1);
    if (lookahead_state == 0){  //If in the next step we would go to NULL state we consider the TOKEN finished
        bool accept_token = is_accepting_state(a, new_state); 
        COUNT_GEN(2);
        COUNT_COMP(1);
        if (accept_token)       //If we are in an accepting state we accept the token
            return ACCEPT_TOKEN;
        return STOP_AUTOMATA;   //Else we would reject the TOKEN and stop the automata
    }
    COUNT_GEN(1);
    return CORRECT_RETURN;
}

/**
 * Driver que processa un fitxer amb múltiples autòmata DFA
 * Escriu tokens reconeguts i no reconeguts en fitxers separats
 */
void automata_driver(AutomataDFA **automata_list, int num_automata){
    
    COUNT_IO(1);  // fgetc reads 1 character
    char c = fgetc(status.ifile); // Caràcter llegit del fitxer
    COUNT_COMP(1);  // if condition
    if (c == EOF){
        report_error_typed(ERR_EMPTY_FILE, 0, SCANNER_STEP);
        return;
    }
    
    char lookahead;
    BufferAuto buffer;
    BufferAuto buffer_nonrecognized;
    buffer_clear(&buffer);
    buffer_clear(&buffer_nonrecognized);
    COUNT_GEN(2);  // buffer_clear calls
    
    Category nonrecognized_category = CAT_NONRECOGNIZED;

    COUNT_GEN(1);  // while loop
    while ((lookahead = fgetc(status.ifile)) != EOF){ //Anar llegint el fitxer
        COUNT_IO(1);
        COUNT_COMP(1); 
        bool accepted = false; // Indica si algun autòmata ha acceptat aquest token
        ActionSkip action = skip_nonchars(c, lookahead);
        c = action.c;
        lookahead = action.lookahead;
        COUNT_GEN(2);
        COUNT_COMP(1);
        if (action.to_do == EOL_RETURN) {
            COUNT_COMP(1); 
            if (status.line_has_tokens) {
                COUNT_COMP(1);
                if (status.oform == RELEASE) {
                    fprintf(status.ofile, "\n");
                    COUNT_IO(1);
                } else if (status.oform == DEBUG) {
                    fprintf(status.ofile, "\n\n"); // línia extra en DEBUG
                    COUNT_IO(1);
                }
            }

            status.first_token_in_line = true;
            status.line_has_tokens = false;
            status.line++;
            COUNT_GEN(3);  // status updates
        }
        COUNT_COMP(1); 
        if(action.to_do == EOF_RETURN){
            break;
        }

        buffer_add(&buffer, c);
        COUNT_GEN(1);

        COUNT_GEN(1);  // for loop initialization
        for (int i = 0; i < num_automata; i++){// Iterar sobre tots els autòmata
            COUNT_COMP(1); 
            COUNT_COMP(1); 
            if (!automata_list[i]->dont_look_anymore){ // Només processar actius
                int decision = update_automata(automata_list[i], c, lookahead);

                COUNT_COMP(1);
                if (decision == ACCEPT_TOKEN){ // L'autòmata ha arribat a un estat d'acceptació 
                    COUNT_COMP(1);
                    if(buffer_nonrecognized.len != 0){
                        write_token_to_file_and_list(&buffer_nonrecognized, nonrecognized_category);
                        buffer_clear(&buffer_nonrecognized);
                        COUNT_GEN(1);
                    }  
                    write_token_to_file_and_list(&buffer, automata_list[i]->type);
                    accepted = true; // Marcar que s'ha acceptat un token
                    COUNT_GEN(1);
                    break;           // Reiniciarem tots els autòmata després
                    
                } else if (decision == STOP_AUTOMATA){ // L'autòmata ha rebutjat, marcar com a "no mirar més"
                    COUNT_COMP(1);
                    automata_list[i]->dont_look_anymore = true; 
                    COUNT_GEN(1);
                }
                // value == -1 → continua processant, no fem res
            }
        }

        COUNT_COMP(1);
        if (accepted){ //Si algun dels automates ha acceptat el token
            buffer_clear(&buffer);
            restart_automatas(automata_list, num_automata);
            COUNT_GEN(1);
        }
        else {
            bool all_done = true;
            for (int i = 0; i < num_automata; i++){ // Comprovar si tots els autòmata han rebutjat el caràcter
                COUNT_COMP(1);
                if (!automata_list[i]->dont_look_anymore){
                    all_done = false;
                    COUNT_GEN(1);
                    break;
                }
            }

            COUNT_COMP(1);
            if (all_done){
                buffer_move_append(&buffer_nonrecognized, &buffer);
                // report_error_typed(ERR_TOKEN_NOT_RECOGNIZED, status.line);
                restart_automatas(automata_list, num_automata);
            }
        }
        c = lookahead; //lookahead becomes the actual char
        COUNT_GEN(1);
    }
    
    if(c != EOF && status.all_tokens.count == 0){ //Case of file with only one character, we do not handle this case
        fprintf(status.ofile, "<%c, %s> ",
                c, category_to_string(CAT_NONRECOGNIZED));
        COUNT_IO(1);
        report_error_typed(ERR_TOKEN_NOT_RECOGNIZED, status.line, SCANNER_STEP);
    }
}