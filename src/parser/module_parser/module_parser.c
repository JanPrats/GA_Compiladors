#include <stdlib.h>
#include <string.h>
#include "module_parser.h"

AutomataSRA* initializeSRA(AutomataDFA* dfa, const ParseTable* table){
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
    sra->tokens = 0;
    //sra->language = language;

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

ParseAction get_next_action(LanguageV2* language, Token token, RuleItemType* type){ //example: input Token = <"9", CAT_NUMBER>
    /*
    Search throughout the vocabulary (the one in the sra inside language)
    For each ( (type, (<lexeme, category>) , column ) inside each vocabulary    // For example : ( (TERMINAL, (<"+", CAT_INDIFERENT>) , 1 )
    Take the category and see if it is of type CAT_INDIFERENT
        if it is: compare input token lexeme with vocab's token lexeme
        else:     compare input token category with vocab's token category      // In our example we would compare "9" with "+" ==> Not equal
    
    When it finds a match, then assign type from vocab to type from input (so that it can be used outside function)
    Then it takes the column and the current_state from sra. 
        If type = Terminal ==>      search in SRA's Table       [action table]
        If type = Non-terminal ==>  search in SRA's DFA's Table [goto table]
    if action table ==>
    Use a function (needs to be created) to detect if it is a accept, reject, shift, reduce and create a ParseAction variable with it's type_of_action and number (from s2 to {SHIFT, 2})
    If goto table ==> 
    Create a ParseAction with {GOTO, next_state}
    return this variable for both cases
    */
    
}

void reduce_rule(Stack* stack, RuleV2* rule, Token *lhs, int *num_tokens){
    /*
    Takes the Rule 
    Looks at its rhs length (a parameter in the struct)
    Pops as many times as rule's rhs length
    Adds lhs symbol(s) to Token lhs* {Since it is a pointer it should change the original array}
    adds lhs_length to num_tokens to know the lhs lenght outside this function
    */
}

ActionType update_automatasra(AutomataSRA *a, Token token, LanguageV2* language) {
    ParseAction action; // [SHIFT, NEXT_TOKEN] or [GOTO, NEXT_TOKEN] or [REDUCE, RULEID]
    RuleItemType type_tnt; //Is our token a terminal or a non-terminal?
    action = get_next_action(a, token, &type_tnt); //Get next action (and assign T or NT to type_tnt)
    ActionType returned = action.type;

    if (action.type == ACTION_SHIFT || action.type == ACTION_GOTO)
    {
        RuleItem ruit; //Item to store in Stack (token + type_tnt)
        ruit.token = token;
        ruit.type = type_tnt;
        push_stack(a->stack, ruit, action.value); //Push item to stack
        a->dfa->current_state = action.value; //Update Automata0s current state
    }
    else if (action.type == ACTION_REDUCE)
    {
        RuleV2 rule2r = language->productions[action.value];
        Token lhs[MAX_RHS_LENGTH];
        int num_tokens;
        
        reduce_rule(a->stack, &rule2r, &lhs, &num_tokens); // pop following rhs

        for (int i = 0; i<=num_tokens; i++){ //Not sure if i<=num_tokens or i<num_tokens | MUST NOT CONSUME LOOKAHEAD
            returned = update_automatasra(a, lhs[i], language);
            if (returned == ACTION_REJECT || returned == ACTION_ERROR){
                return returned;
            }
        }
    }
    
    token.line = a->tokens; //since we won't be using token.line anymore we substitute it by the position on the list where we found this token (can be used to determine where to puta | or · in the output <input> column. F.example { 9 * | ( 5 + 2 )} )
    write_update_to_output(*language->sra->stack, token, action); // See function for explanation. stack --> State, tokn --> input, stack --> stack, decisiton --> operation
    return returned;
}


Token read_next_token(AutomataSRA sra, int* returned){ //This input parameter could be changed depending on what we want
    //Read the handout first, because I'm writting this from what I can recall, but I may be wrong.
    //If we are given a list in memory it reads the next element in the list

    //If we are given a file, we either call a function that reads <lexeme, category> pattern each time we want to read a token
    // Or we put all the file into a list in memory at a time and use the other method.
    
    int count = sra.tokens;         // count = [0,1,...,N-1] where count is the position in the List of Tokens that our sra is at.
    sra.tokens = sra.tokens + 1;    // count++ {since we have consumed a token}

    returned = CORRECT_RETURN;      // Unless proved wrong, we are reading fine

    if (count >= status.all_tokens.count){ //If count ==
        returned == EOTokenList;
    }

    return status.all_tokens.tokens[count];
}

int write_update_to_output(Stack stack, Token tokn, ParseAction op){
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
void automatasra_driver(LanguageV2 * language){
    int returned;
    Token tokn = read_next_token(*language->sra, &returned); // Caràcter llegit del fitxer

    if (returned == EOTokenList){
        report_error_typed(ERR_EMPTY_FILE, 0, SCANNER_STEP);
        return;
    }

    int returned_copy = returned;

    while (returned != EOTokenList){ //Anar llegint el fitxer
        
        ActionType operation = update_automatasra(language->sra, tokn, language);

        if (operation != ACTION_REJECT){ // L'autòmata ha arribat a un estat d'acceptació 
            int a = 0;
            
        } else { // L'autòmata ha rebutjat, marcar com a "no mirar més"
            int a = 0;
            break;
        }
        tokn = read_next_token(*language->sra, &returned);
    }
    
    if(returned_copy != EOTokenList && language->sra->tokens == 0){ //Case of file with only one character, we do not handle this case
        report_error_typed(ERR_TOKEN_NOT_RECOGNIZED, status.line, SCANNER_STEP);
    }
}
