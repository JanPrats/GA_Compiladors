#include <stdlib.h>
#include <string.h>
#include "module_parser.h"

AutomataSRA* initializeSRA(AutomataDFA* dfa, const ParseTable* table, ListTokens* tokens, Language* language){
    if (dfa == NULL || table == NULL) return NULL;
    AutomataSRA* sra = (AutomataSRA*)malloc(sizeof(AutomataSRA)); //guardem memoria per SRA i afegim tots els elements que necessita
    if (!sra) return NULL;
    //Set DFA pointer
    sra->dfa = dfa;
    //Copy parse table by value
    sra->table = *table;
    //Allocate and initialize stack
    sra->stack = (Stack*)malloc(sizeof(Stack));
    if (sra->stack == NULL){
        free(sra);
        return NULL;
    }
    initialize_stack(sra->stack, *dfa);
    //Set tokens and language pointers
    sra->tokens = tokens;
    sra->language = language;

    return sra; //Return pointer to the initialized SRA with all parameters set
}

//Free resources owned by an AutomataSRA allocated with initializeSRA. Free the stack and the SRA itself, but not the DFA, tokens or language.
void destroySRA(AutomataSRA* sra){
    if (sra == NULL) return;

    if (sra->stack != NULL){
        free(sra->stack);
        sra->stack = NULL;
    }

    free(sra);
}

ActionType update_automata(AutomataSRA *a, Token tokn, Token lookahead) {
    
    // Two_ints acala = search_two_columns(a, c, lookahead); //Search the column of TransitionMatrix of both current character and lookahead character

    // int new_state = a->matrix.states_rows[a->current_state].new_state[acala.actual]; 

    // a->current_state = new_state; //Update Automata

    // if (new_state == 0)             //This can only happen if the character is the first to be put from initial state. So we would already stop the automata,
    //     return STOP_AUTOMATA;       // In any other case we would have seen the Lookahead going to NULL state

    // int lookahead_state = a->matrix.states_rows[new_state].new_state[acala.lookahead]; //See where we would go in the next step (with the lookahead)

    // if (lookahead_state == 0){  //If in the next step we would go to NULL state we consider the TOKEN finished
    //     bool accept_token = is_accepting_state(a, new_state); 
    //     COUNT_GEN(2);
    //     COUNT_COMP(1);
    //     if (accept_token)       //If we are in an accepting state we accept the token
    //         return ACCEPT_TOKEN;
    //     return STOP_AUTOMATA;   //Else we would reject the TOKEN and stop the automata
    // }
    // return CORRECT_RETURN;
}


Token read_next_token(ListTokens * tokenlist, int* returned){ //This input parameter could be changed depending on what we want
    //Read the handout first, because I'm writting this from what I can recall, but I may be wrong.
    //If we are given a list in memory it reads the next element in the list

    //If we are given a file, we either call a function that reads <lexeme, category> pattern each time we want to read a token
    // Or we put all the file into a list in memory at a time and use the other method.
    returned = CORRECT_RETURN;
}

int write_update_to_output(Stack stack, Token tokn, ActionType op){
    /*
    "Adds a row to the output table"

    -------------------------------------
    | State | Input | Stack | Operation |
    -------------------------------------
    |   0   | int   |  E    | Reduce    |
    -------------------------------------
    */
    // Ja ens encarregarem de que es vegi bonic després
    StackElement temporal_element = peek_stack(&stack);
    int state = temporal_element.state;
    char input[MAX_INPUT_LENGTH];
    //L'input molaria tenir com una llista interna dels elements llegits i que posi un | al lloc on estim llegint ara (com a la slide 21 de la T4)
    char stack[MAX_INPUT_LENGTH];
    //Hauriem de cridar una funció que passi l'stack de Stack a string
    char operation[MAX_OPERATION_NAME];
    //Hauriem de cridar una funció que passi de ActionType a String

    //fprintf(outputfile, "| ",state, " | ", input[MAX_INPUT_LENGTH], " | ", char stack[MAX_INPUT_LENGTH], " | ", char operation[MAX_OPERATION_NAME]" |",);
    return CORRECT_RETURN;
}

/**
 * Driver que processa un fitxer amb múltiples autòmata DFA
 * Escriu tokens reconeguts i no reconeguts en fitxers separats
 */
void automata_driver(AutomataSRA *automata_sra){
    int returned;
    Token tokn = read_next_token(automata_sra->tokens, &returned); // Caràcter llegit del fitxer

    if (returned == EOTokenList){
        report_error_typed(ERR_EMPTY_FILE, 0, SCANNER_STEP);
        return;
    }
    int returned_copy = returned;
    
    Token lookahead = read_next_token(automata_sra->tokens, &returned); // Caràcter llegit del fitxer
    // BufferAuto buffer;
    // BufferAuto buffer_nonrecognized;
    // buffer_clear(&buffer);
    // buffer_clear(&buffer_nonrecognized);
    
    // Category nonrecognized_category = CAT_NONRECOGNIZED;

    while (returned != EOTokenList){ //Anar llegint el fitxer
        // bool accepted = false; // Indica si algun autòmata ha acceptat aquest token
        //buffer_add(&buffer, c);

        
        ActionType operation = update_automata(automata_sra, tokn, lookahead);

        if (operation != ACTION_REJECT){ // L'autòmata ha arribat a un estat d'acceptació 

            // if(buffer_nonrecognized.len != 0){
            //     write_token_to_file_and_list(&buffer_nonrecognized, nonrecognized_category);
            //     buffer_clear(&buffer_nonrecognized);
            // }  
            write_update_to_output(*automata_sra->stack, tokn, operation); // See function for explanation. stack --> State, tokn --> input, stack --> stack, decisiton --> operation
            // accepted = true; // Marcar que s'ha acceptat un token
            // break;           // Reiniciarem tots els autòmata després
            
        } else { // L'autòmata ha rebutjat, marcar com a "no mirar més"
            write_update_to_output(*automata_sra->stack, tokn, operation); // See function for explanation. stack --> State, tokn --> input, stack --> stack, decisiton --> operation
            break;
        }

        // if (accepted){ //Si algun dels automates ha acceptat el token
        //     buffer_clear(&buffer);
        //     restart_automatas(automata_sra, num_automata);
        // }
        // else {
        //     bool all_done = true;
        //     for (int i = 0; i < num_automata; i++){ // Comprovar si tots els autòmata han rebutjat el caràcter

        //         if (!automata_sra[i]->dont_look_anymore){
        //             all_done = false;
        //             break;
        //         }
        //     }

        //     if (all_done){
        //         buffer_move_append(&buffer_nonrecognized, &buffer);
        //         // report_error_typed(ERR_TOKEN_NOT_RECOGNIZED, status.line);
        //         restart_automatas(automata_sra, num_automata);
        //     }
        // }
        tokn = lookahead; //lookahead becomes the actual char
    }
    
    if(returned_copy != EOTokenList && automata_sra->tokens->count == 0){ //Case of file with only one character, we do not handle this case
        report_error_typed(ERR_TOKEN_NOT_RECOGNIZED, status.line, SCANNER_STEP);
    }
}
