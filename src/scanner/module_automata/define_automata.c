#include <string.h>
#include "module_automata.h"

AutomataDFA keyword_automata = {
    .alphabet = { 'i', 'f', 'o', 'r', 'w', 'h', 'l', 'e', 's' },
    .states = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12},
    .start_state = 1,
    .current_state = 1,
    .accepting_states = { 12 },
    .lookahead_acceptance = {  '(', '{',  '\n', '\t' },
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

AutomataDFA type_automata = {
    .alphabet = { 'i', 'n', 't', 'c', 'h', 'a', 'r', 'v', 'o', 'd' },
    .states = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },
    .start_state = 1,
    .current_state = 1,
    .accepting_states = { 10 },
    .lookahead_acceptance = {  '*', '\n', '\t', ')' },
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


//primera versió dels literals pero encara no es correcte
AutomataDFA literal_automata = {
    .alphabet = { '"' },
    .states = {0, 1, 2, 3},
    .start_state = 0,
    .current_state = 0,
    .accepting_states = {2},
    .lookahead_acceptance = { ' ', ';', '\n', '\t', '(', '{', '[', ']', ',', '.', ')', '}', '+', '=', '*', '>' },
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