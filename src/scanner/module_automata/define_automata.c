#include <string.h>
#include "module_automata.h"


AutomataDFA identifer_automata = {
    .alphabet = {
        // Minúsculas (1)   [a-z]
        {'a', 1}, {'b', 1}, {'c', 1}, {'d', 1}, {'e', 1}, {'f', 1}, {'g', 1}, {'h', 1}, {'i', 1},
        {'j', 1}, {'k', 1}, {'l', 1}, {'m', 1}, {'n', 1}, {'ñ', 1}, {'o', 1}, {'p', 1}, {'q', 1},
        {'r', 1}, {'s', 1}, {'t', 1}, {'u', 1}, {'v', 1}, {'w', 1}, {'x', 1}, {'y', 1}, {'z', 1},
        
        // Mayúsculas (1)   [A-Z]
        {'A', 1}, {'B', 1}, {'C', 1}, {'D', 1}, {'E', 1}, {'F', 1}, {'G', 1}, {'H', 1}, {'I', 1},
        {'J', 1}, {'K', 1}, {'L', 1}, {'M', 1}, {'N', 1}, {'Ñ', 1}, {'O', 1}, {'P', 1}, {'Q', 1},
        {'R', 1}, {'S', 1}, {'T', 1}, {'U', 1}, {'V', 1}, {'W', 1}, {'X', 1}, {'Y', 1}, {'Z', 1},
        
        // Dígitos (2)      [0-9]
        {'0', 2}, {'1', 2}, {'2', 2}, {'3', 2}, {'4', 2}, {'5', 2}, {'6', 2}, {'7', 2}, {'8', 2}, {'9', 2},
        
        // Otros caracteres (0)
        {' ', 0}, {'\t', 0}, {'\n', 0}, {'\r', 0},
        {'.', 0}, {',', 0}, {';', 0}, {':', 0},
        {'!', 0}, {'¡', 0}, {'?', 0}, {'¿', 0},
        {'"', 0}, {'\'', 0}, {'`', 0},
        {'(', 0}, {')', 0}, {'[', 0}, {']', 0}, {'{', 0}, {'}', 0},
        {'+', 0}, {'-', 0}, {'*', 0}, {'/', 0}, {'%', 0},
        {'=', 0}, {'<', 0}, {'>', 0},
        {'&', 0}, {'|', 0}, {'^', 0}, {'~', 0},
        {'@', 0}, {'#', 0}, {'$', 0}, {'_', 0},
        {'\\', 0}
    },
    .states = { 0, 1, 2 },
    .start_state = 1,
    .current_state = 1,
    .accepting_states = { 2 },
    .matrix = {
        .states_rows = {//  [Other Characters] [a-zA-Z] [0-9]
            { .new_state = { 0, 0, 0 } }, // [Ø state] (NULL state)
            { .new_state = { 0, 2, 0 } }, // [start state]
            { .new_state = { 0, 2, 2 } }  // [accepting state]
        },
        .width = 3,   
        .height = 3,   
    },
    .type = CAT_IDENTIFIER,
    .dont_look_anymore = false
};

/*
Maybe this would be more efficient:
for (char c = 'a'; c <= 'z'; c++) alphabet[i++] = (SymbolVocab){c, COL_LETTER};
for (char c = 'A'; c <= 'Z'; c++) alphabet[i++] = (SymbolVocab){c, COL_LETTER};
for (char c = '0'; c <= '9'; c++) alphabet[i++] = (SymbolVocab){c, COL_DIGIT};

but it's just to make it fancy tbh, lets leave it like this
*/

AutomataDFA keyword_automata = {
    .alphabet = { 'i', 'f', 'o', 'r', 'w', 'h', 'l', 'e', 's' },
    .states = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12},
    .start_state = 1,
    .current_state = 1,
    .accepting_states = { 12 },
    .matrix = {
        .states_rows = {
            { .new_state = { 2, 10, 0, 0, 6, 0, 0, 3, 0 } }, 
            { .new_state = { 0, 12, 0, 0, 0, 0, 0, 0, 0 } }, 
            { .new_state = { 0, 0, 0, 0, 0, 0, 4, 0, 0 } }, 
            { .new_state = { 0, 0, 0, 0, 0, 0, 0, 0, 5 } }, 
            { .new_state = { 0, 0, 0, 0, 0, 0, 0, 12, 0 } }, 
            { .new_state = { 0, 0, 0, 0, 0, 7, 0, 0, 0 } }, 
            { .new_state = { 8, 0, 0, 0, 0, 0, 0, 0, 0 } },
            { .new_state = { 0, 0, 0, 0, 0, 0, 9, 0, 0 } }, 
            { .new_state = { 0, 0, 0, 0, 0, 0, 0, 12, 0 } },
            { .new_state = { 0, 11, 0, 0, 0, 0, 0, 0, 0 } },
            { .new_state = { 0, 0, 12, 0, 0, 0, 0, 0, 0 } },
            { .new_state = { 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
            { .new_state = { 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
        },
        .width = 9,   
        .height = 13   
    },
    .type = CAT_KEYWORD,
    .dont_look_anymore = false
};

AutomataDFA return_automata = {
    .alphabet = { 'r', 'e', 't', 'u', 'n' },
    .states = { 0, 1, 2, 3, 4, 5, 6, 7 },
    .start_state = 1,
    .current_state = 1,
    .accepting_states = { 7 },
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

AutomataDFA type_automata = {
    .alphabet = { 'i', 'n', 't', 'c', 'h', 'a', 'r', 'v', 'o', 'd' },
    .states = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },
    .start_state = 1,
    .current_state = 1,
    .accepting_states = { 10 },
    .matrix = {
        .states_rows = {
            { .new_state = { 2, 0, 0, 4, 0, 0, 0, 7, 0, 0 } }, 
            { .new_state = { 0, 3, 0, 0, 0, 0, 0, 0, 0, 0 } }, 
            { .new_state = { 0, 0, 10, 0, 0, 0, 0, 0, 0, 0 } }, 
            { .new_state = { 0, 0, 0, 0, 5, 0, 0, 0, 0, 0 } }, 
            { .new_state = { 0, 0, 0, 0, 0, 6, 0, 0, 0, 0 } }, 
            { .new_state = { 0, 0, 0, 0, 0, 0, 10, 0, 0, 0 } }, 
            { .new_state = { 0, 0, 0, 0, 0, 0, 0, 0, 8, 0 } }, 
            { .new_state = { 9, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, 
            { .new_state = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, 
            { .new_state = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, 
            { .new_state = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
        },
        .width = 10,   
        .height = 11,   
    },
    .type = CAT_TYPE,
    .dont_look_anymore = false
};


AutomataDFA number_automata = {
    .alphabet = { '0','1','2','3','4','5','6','7','8','9' },
    .states = { 1, 2, 3 },        
    .start_state = 1,
    .current_state = 1,
    .accepting_states = { 2 },
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


//primera versió dels literals pero encara no es correcte
AutomataDFA literal_automata = {
    .alphabet = { '"' },
    .states = {0, 1, 2, 3},
    .start_state = 0,
    .current_state = 0,
    .accepting_states = {2},
    .matrix = {
        .states_rows = {
            { .new_state = { 1 } },  
            { .new_state = { 2 } },  
            { .new_state = { 3 } },  
            { .new_state = { 3 } }   
        },
        .width = 1,
        .height = 4
    },
    .type = CAT_LITERAL,
    .dont_look_anymore = false
};