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
