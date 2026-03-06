#include <stdlib.h>
#include <string.h>
#include "module_parser.h"

void initialize_SRA_DFA_stack(LanguageV2* language) {
    language->sra->dfa->start_state   = 0;
    language->sra->dfa->current_state = 0;
    initialize_stack(language->sra->stack, *language->sra->dfa);
}

//Free resources owned by an AutomataSRA allocated with initializeSRA. Free the stack and the SRA itself, but not the DFA, tokens or language.
void destroy_language(LanguageV2* language) {
    if (language == NULL) return;

    if (language->sra != NULL) {
        if (language->sra->dfa != NULL) {
            free(language->sra->dfa);
            language->sra->dfa = NULL;
        }
        if (language->sra->stack != NULL) {
            destroy_stack(language->sra->stack);
            free(language->sra->stack);
            language->sra->stack = NULL;
        }
        free(language->sra);
        language->sra = NULL;
    }
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
    
    
    int current_state = language->sra->dfa->current_state;
    ParseAction result;
    result.type  = ACTION_ERROR;
    result.value = 0;

    // ---- Search terminals ------------------------------------------------
    for (int i = 0; i < language->num_terminals; i++) {
        RuleItem vocab = language->terminals[i];
        // vocab.token.cat == CAT_INDIFERENT  -->  match by lexeme
        // otherwise                          -->  match by category
        int match = 0;
        if (vocab.token.cat == CAT_INDIFERENT) {
            match = (strcmp(token.lexeme, vocab.token.lexeme) == 0);
        } else {
            match = (token.cat == vocab.token.cat);
        }

        if (match) {
            *type = TERMINAL_SYMBOL;
            int col = vocab.column;
            // Look up in the ACTION table
            ParseAction cell = language->sra->table.cells[current_state][col]; // err, Reject_Error {} Check if languange.txt transforms err from table into ERROR or REJECT
            return cell;   // already has type + value populated when the table was built
        }
    }
    // fprintf(status.ofile, "DEBUG: no terminal match for lexeme='%s' cat=%d, state=%d\n", 
    // token.lexeme, token.cat, current_state);
    // for (int i = 0; i < language->num_terminals; i++) {
    //     fprintf(status.ofile, "  vocab[%d]: lexeme='%s' cat=%d col=%d\n",
    //         i, language->terminals[i].token.lexeme, 
    //         language->terminals[i].token.cat,
    //         language->terminals[i].column);
    // }

    // ---- Search non-terminals -------------------------------------------
    for (int i = 0; i < language->num_nonterminals; i++) {
        RuleItem vocab = language->nonterminals[i][0]; // first slot holds the NT descriptor // Check if language.txt is correct here aswell
        int match = 0;
        if (vocab.token.cat == CAT_INDIFERENT) {
            match = (strcmp(token.lexeme, vocab.token.lexeme) == 0);
        } else {
            match = (token.cat == vocab.token.cat);
        }

        if (match) {
            *type = NON_TERMINAL_SYMBOL;
            int col = vocab.column;
            // Look up in the GOTO table (stored in the DFA's transition matrix)
            int next_state = language->sra->dfa->matrix.states_rows[current_state].new_state[col];
            result.type  = ACTION_GOTO; //since we are looking at the goto table
            result.value = next_state;
            return result;
        }
    }

    // No match found: return error
    return result;
}

void reduce_rule(Stack* stack, RuleV2* rule, Token *lhs, int *num_tokens, LanguageV2 * language){
    /*
    Takes the Rule 
    Looks at its rhs length (a parameter in the struct)
    Pops as many times as rule's rhs length
    Adds lhs symbol(s) to Token lhs* {Since it is a pointer it should change the original array}
    adds lhs_length to num_tokens to know the lhs lenght outside this function
    */
    
    // Pop rhs_length elements from the stack (one per symbol on the RHS)
    for (int i = 0; i < rule->rhs_length; i++) {
        pop_stack(stack);
    }

    // Fill the output lhs array with the tokens from the rule's LHS RuleItems
    for (int i = 0; i < rule->lhs_length; i++) {
        lhs[i] = rule->lhs[i].token;  // copy the Token part of each LHS RuleItem
    }

    // Tell the caller how many LHS tokens were produced
    *num_tokens = rule->lhs_length;

    //Backtrack
    StackElement top = peek_stack(stack);
    
    language->sra->dfa->current_state = top.state;
}

ActionType update_automatasra(AutomataSRA *a, Token token, LanguageV2* language) {
    ParseAction action; // [SHIFT, NEXT_STATE] or [GOTO, NEXT_STATE] or [REDUCE, RULEID]
    RuleItemType type_tnt; //Is our token a terminal or a non-terminal?
    action = get_next_action(language, token, &type_tnt); //Get next action (and assign T or NT to type_tnt)
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
        RuleV2 rule2r;
        int found = 0;
        for (int i = 0; i < language->num_productions; i++) {
            if (language->productions[i].rule_id == action.value) {
                rule2r = language->productions[i];
                found = 1;
                break;
            }
        }
        Token lhs[MAX_RHS_LENGTH];
        int num_tokens;
        
        reduce_rule(a->stack, &rule2r, lhs, &num_tokens, language); // pop following rhs
        // printf("REDUCE rule %d, LHS='%s', current_state after reduce=%d, token='%s'\n",
        //     action.value,
        //     lhs[0].lexeme,
        //     a->dfa->current_state,
        //     token.lexeme);
        // include the lookahead/original token after the reduced lhs symbols
        // this ensures the token is processed as part of the recursive calls 
        // if (num_tokens < MAX_RHS_LENGTH) {
        //     lhs[num_tokens] = token;
        //     num_tokens++;
        // }

        // process each token produced by the reduction (including the original token)
        token.line = a->tokens; 
        write_update_to_output(*language->sra->stack, token, action);
        for (int i = 0; i < num_tokens; i++) {
            returned = update_automatasra(a, lhs[i], language);
            if (returned == ACTION_REJECT || returned == ACTION_ERROR) {
                return returned;
            }
        }

        // Now re-process the original token in the new state (tail call, not recursion on lhs)
        return update_automatasra(a, token, language);
    }
    
    token.line = a->tokens; //since we won't be using token.line anymore we substitute it by the position on the list where we found this token (can be used to determine where to puta | or · in the output <input> column. F.example { 9 * | ( 5 + 2 )} )
    write_update_to_output(*language->sra->stack, token, action); // See function for explanation. stack --> State, tokn --> input, stack --> stack, decisiton --> operation
    return returned;
}


Token read_next_token(AutomataSRA* sra, int* returned){ //This input parameter could be changed depending on what we want
    //Read the handout first, because I'm writting this from what I can recall, but I may be wrong.
    //If we are given a list in memory it reads the next element in the list

    //If we are given a file, we either call a function that reads <lexeme, category> pattern each time we want to read a token
    // Or we put all the file into a list in memory at a time and use the other method.
    
    int count = sra->tokens;         // count = [0,1,...,N-1] where count is the position in the List of Tokens that our sra is at.
    sra->tokens = sra->tokens + 1;    // count++ {since we have consumed a token}

    if (returned)
        *returned = CORRECT_RETURN;      // Unless proved wrong, we are reading fine

    if (count >= status.all_tokens.count){ //If count >= globaltokenlist.count (if globaltokenlist.count == k ; it contains k elements so list[k-1] contains the last element)
        if (returned)
            *returned = EOTokenList;
        Token eof;
        strncpy(eof.lexeme, EOF_TOKEN_LEXEME, MAX_TOKEN_NAME-1); // we could also use EOF_TOKEN_LEXEME // Check if in language.txt $ ==> EOF_TOKEN_LEXEME
        eof.lexeme[MAX_TOKEN_NAME-1] = '\0'; 
        eof.line = sra->tokens;
        eof.cat = CAT_INDIFERENT;
        return eof;
    }

    return status.all_tokens.tokens[count];
}

int write_update_to_output(Stack stack, Token tokn, ParseAction op){
    /*
    "Adds a row to the output table"

    --------------------------------------------
    | State | Input | Stack | Operation        |
    --------------------------------------------
    |   1   | int   |  E    | Reduce 5 or R5   |
    --------------------------------------------
    */
    // Get the state from the top of the stack
    StackElement temporal_element = peek_stack(&stack);
    int state = temporal_element.state;
    
    // Build the input string (tokens read so far with a | marker)
    char input[MAX_INPUT_LENGTH];
    input[0] = '\0';
    
    // Add all tokens read so far up to the current token position
    for (int i = 0; i < status.all_tokens.count && i < tokn.line; i++) {
        if (i > 0 && strlen(input) + 1 < MAX_INPUT_LENGTH - 1)
            strcat(input, " ");
        if (strlen(input) + strlen(status.all_tokens.tokens[i].lexeme) < MAX_INPUT_LENGTH - 1)
            strcat(input, status.all_tokens.tokens[i].lexeme);
    }
    // // Add the marker for current position
    // if (tokn.line < status.all_tokens.count) {
    //     if (strlen(input) + 3 < MAX_INPUT_LENGTH - 1)
    //         strcat(input, " | ");
    //     if (strlen(input) + strlen(tokn.lexeme) < MAX_INPUT_LENGTH - 1)
    //         strcat(input, tokn.lexeme);
    // } else if (tokn.line == status.all_tokens.count) {
    //     if (strlen(input) + 3 < MAX_INPUT_LENGTH - 1)
    //         strcat(input, " | ");
    // }
    
    // Get stack representation (from 0 to top)
    char stack_str[MAX_INPUT_LENGTH];
    stack_to_string(&stack, stack_str, sizeof(stack_str));
    
    // Get operation representation (e.g., S6, R7, Accept, Reject)
    char operation[MAX_OPERATION_NAME];
    action_to_string(op, operation, sizeof(operation));
    
    // Write the formatted row to output file
    // Format: | State | Input | Read Token | Stack | Action |
    fprintf(status.ofile, "| %5d | %-20s | %-10s | %-20s | %-10s |\n", 
            state, 
            input, 
            tokn.lexeme,
            stack_str, 
            operation);
    
    return CORRECT_RETURN;
}

/**
 * Driver que processa un fitxer amb múltiples autòmata DFA
 * Escriu tokens reconeguts i no reconeguts en fitxers separats
 */
void automatasra_driver(LanguageV2 * language){
    int returned;
    Token tokn = read_next_token(language->sra, &returned); // Caràcter llegit del fitxer

    if (returned == EOTokenList){
        //Error
        return;
    }
    // int returned_copy = returned;
    ActionType operation = ACTION_ACCEPT; //Not sure if accept or reject if empty file

    while (returned != EOTokenList){ //Anar llegint el fitxer
        // fprintf(status.ofile, "We have reached this point 2\n");
        ActionType operation = update_automatasra(language->sra, tokn, language);
        // ParseAction pact;
        // pact.type = operation;
        // pact.value = -1; //change by global variable
        // fprintf(status.ofile, "We have reached this point 3\n");
        // char oper[MAX_OPERATION_NAME];
        // action_to_string(pact, oper, sizeof(operation));
        // fprintf(status.ofile, "action: %s\n", oper);
        // fprintf(status.ofile, "Ehmm %d\n", operation == ACTION_REJECT);

        if (operation == ACTION_REJECT || operation == ACTION_ERROR) { // L'autòmata ha rebutjat la llista de tokens
            break;
        }
        // fprintf(status.ofile, "We have reached this point %d", language->sra->tokens);
        tokn = read_next_token(language->sra, &returned); //Last token will be EOF token
    }

    if (update_automatasra(language->sra, tokn, language) != ACTION_ACCEPT){ //Last update with EOF ($)
        ParseAction pact;
        pact.type = ACTION_REJECT;
        pact.value = -1; //change by global variable
        write_update_to_output(*language->sra->stack, tokn, pact);
    }
    
    // if(returned_copy != EOTokenList && language->sra->tokens == 0){ //Case of file with only one character, we do not handle this case
    //     report_error_typed(ERR_TOKEN_NOT_RECOGNIZED, status.line, SCANNER_STEP);
    // }
}
