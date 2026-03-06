/*
 * -----------------------------------------------------------------------------
 * config.h
 *
 * Configuration header for the Scanner module.
 * Defines global constants, flags, and data structures used throughout
 * the scanning and lexical analysis process.
 *
 * Key Definitions:
 * - Debug and output format flags (DEBUG_F, OUTFORMAT_M)
 * - Character constants and parsing limits
 * - Maximum lengths for tokens, files, and buffers
 * - Global status structure declaration
 * - Token and buffer data structures
 *
 * Team: GA
 * Contributor/s: Pol García, Clara Serra
 * -----------------------------------------------------------------------------
 */

#ifndef CONFIG_H
#define CONFIG_H
 
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>     // To set env for time zone
#include <string.h>     // For filename manipulation    
#include <time.h>       // For time-stamp of output logs filenames
#include <ctype.h>





// ------------------COMPILE FLAGS------------------------------------------------- 

#define DEBUG_F ON //On or OFF (defined below, I hope it is not a problem)
#define OUTFORMAT_M DEBUG //MODE of outformat (RELEASE or DEBUG)
#define COUNTOUT_F OUT //Explained below [OUT or DBGCOUNT] (in RELEASE Mode we not count)
#define ON 1        //FOR DEBUG_F
#define OFF 0       //FOR DEBUG_F
#define OUT 1       //FOR COUNTOUT_F
#define DBGCOUNT 0  //FOR COUNTOUT_F
#define HELP_F "-help"
#define ERRORS_F "-errors"

#define PARSER_F false //Should not change until P3 (it will either continue with the parser or not) [IGNORE FOR NOW]


// -----------------CHARACTER CONSTANTS------------------------------------------------- 
#define SPACE_CHAR " "
#define TAB_CHAR "\t"
#define END_OF_LINE "\n"
#define CARRIAGE_RETURN "\r"
#define EPSILON "ε"

#define EOF_TOKEN_LEXEME "$end"


// -----------------SIZE LIMITS------------------------------------------------ 
#define MAX_FILENAME 512        // Max File length (in bits I think) for compiler variables
#define MAX_TOKEN_NAME 4096      // Max Key Length
#define MAX_TOKENS 1024         // Max number of macros
#define MAX_LINE_LENGTH 4096    // Max length of a whole line
#define MAX_FUNCTION_NAME 512
#define MAX_ALPHABET_SIZE 512
#define MAX_STATES 512
#define MAX_AUTOMATAS 256
#define MAXFILENAME 256 // Maximum length of the filename for output logs
#define MAXFILEEXT 64   // Maximum length of the file extension
#define MAX_RHS_LENGTH 20 // Maximum number of symbols on the right-hand side of a production rule
#define MAX_PRODUCTIONS 100 // Maximum number of production rules in the grammar
#define MAX_STACK_SIZE 512 // Maximum size of the parsing stack
#define MAX_SYMBOL_LEN 64
#define MAX_INPUT_LENGTH 1024
#define MAX_OPERATION_NAME 256

// -----------------RETURN VALUES------------------------------------------------ 
#define ERROR_RETURN -1
#define CORRECT_RETURN 0
#define HELP_RETURN 1

#define EOL_RETURN 4
#define EOF_RETURN 5
#define EOTokenList 6

// #define NOT_REJECTED 7
#define NOT_REJECTED 8



//------------------ENUMS-------------------------------------------------------

//error
typedef enum{
    ERR_NONE = 0,
    ERR_INVALID_ARGUMENT,
    ERR_FILE_NOT_FOUND,
    ERR_TOKEN_NOT_RECOGNIZED,
    ERR_TOKEN_TOO_LONG,
    ERR_UNTERMINATED_LITERAL,
    ERR_MAX_TOKENS_EXCEEDED,
    ERR_EMPTY_FILE
} Error;

typedef enum{ 
    SCANNER_STEP,
    PARSER_STEP
} Step;


//Token Types
typedef enum{
    CAT_NUMBER,         //[0-9]+
    CAT_IDENTIFIER,     //[a-zA-Z] ([0-9] or [a-zA-Z])* {not repeated and not a keyword}
    CAT_KEYWORD,        //[a-zA-Z]+ (not sure if there can be numbers in keywords)
    CAT_TYPE,           //[a-zA-Z]+ (not sure if there can be numbers in keywords)
	CAT_LITERAL,        // " ASCII " (really any string between "")
	CAT_OPERATOR,       // = or > or + or *         ( i guess not - nor / nor < nor =< etcetc)
	CAT_SPECIALCHAR,    // ( or ) or ; or { or } or [ or ] or ,
	CAT_NONRECOGNIZED,   // Does not fit in any of the previous
    CAT_INDIFERENT
} Category;

typedef enum {  //Output format 
    RELEASE,
    DEBUG
} Outformat;

typedef enum {
    ACTION_ERROR = 0,  // (Error sintáctico)
    ACTION_SHIFT,      // sX: shift and go to state X
    ACTION_REDUCE,     // rY: Reduce using production Y
    ACTION_GOTO,       // Z: Go to state Z (used after a reduce).
    ACTION_ACCEPT,      // acc
    ACTION_REJECT
} ActionType;

// ------------------Token and token list------------------------------------------------- 

typedef struct Token {
    char lexeme[MAX_TOKEN_NAME];    //Literal string (lexeme)    
    Category cat;                   //Category from the ones above
    int line;                       //Line number where the token starts
} Token;

typedef struct ListTokens {
    Token tokens[MAX_TOKENS];
    int count;
    int pos;                    // current read head — advances on each shift //Well it is also in sra, since we could maybe use more that 1 automata sra at a time in the future
} ListTokens;

// ------------------Language------------------------------------------------- 
// typedef struct Rule {
//     int rule_id;              
//     char lhs[MAX_TOKEN_NAME]; //Left-Hand Side (Ej: "E")
//     char rhs[MAX_RHS_LENGTH][MAX_TOKEN_NAME]; //Right-Hand Side (Ej: "E + T" --> rhs[0] = "E", rhs[1] = "+", rhs[2] = "T")
//     int rhs_length; //PER SABER EL NUM D POPS(Ej: 3 EN "E + T")
// } Rule;

typedef enum {
    TERMINAL_SYMBOL,
    NON_TERMINAL_SYMBOL
} RuleItemType;

typedef struct RuleItem {
    RuleItemType type;
    Token token; //Si lexeme te algun caracter mirarem el caracter, si està vuit llavors mirarem la category
    int column;
} RuleItem;

typedef struct RuleV2 {
    int rule_id;              
    RuleItem lhs[MAX_RHS_LENGTH]; //Left-Hand Side (Ej: "E")
    RuleItem rhs[MAX_RHS_LENGTH]; //Right-Hand Side (Ej: "E + T" --> rhs[0] = "E", rhs[1] = "+", rhs[2] = "T")
    int lhs_length; //PER SABER EL NUM D PUSH
    int rhs_length; //PER SABER EL NUM D POPS(Ej: 3 EN "E + T")
} RuleV2;



// typedef struct Language {
//     char terminals[MAX_ALPHABET_SIZE][MAX_TOKEN_NAME];
//     int  num_terminals;
//     char nonterminals[MAX_ALPHABET_SIZE][MAX_TOKEN_NAME];
//     int  num_nonterminals;
//     Rule productions[MAX_PRODUCTIONS];
//     int  num_productions;
//     char start_symbol[MAX_TOKEN_NAME];
// } Language;


//------------------Parse table and DFA-------------------------------------------------
typedef struct {
    ActionType type; //SHIFT, REDUCE, GOTO, ACCEPT, ERROR
    int value; //Si es SHIFT/GOTO: num state. Si es REDUCE: num regla.
} ParseAction;

// CreC Q HAURIEM DE CANVIAR i utilitzar la taula del dfa, pel q va dir la dolors!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

typedef struct ParseTable {
    ParseAction cells[MAX_STATES][MAX_ALPHABET_SIZE];
    int         num_states;
    int         num_symbols;    // num_terminals + num_nonterminals
} ParseTable;

// ------------------Stack------------------------------------------------
typedef struct StackElement {
    RuleItem symbol;
    int  state;
} StackElement;

// [0, +, 1, * 3]

// [(+,1), (*,3)]

typedef struct Stack{
    StackElement elements[MAX_STACK_SIZE]; //There is an approach where you push jsut the symbol and then the state so [0 'T' 1 '+' 2 'E' 3], but we will do something more similar to [['T', 1]['+',2]['E',3]]
    int top; //Position of the top element in stack [0,1,2,3]
} Stack;

//------------------DFA and Automata-------------------------------------------------
// typedef struct TableElement {
//     int next_state;
//     Rule production_rule;
// } TableElement;

// typedef struct TransitionMatrix {
//     TableElement cells[MAX_STATES][MAX_ALPHABET_SIZE];
// } TransitionMatrix;
typedef struct StateRow {
    int new_state[MAX_ALPHABET_SIZE]; // Contains a state_number for each state-char pair S0_row = [1,0,3,0,2] (assume symbols from the alphabet are always in the same order)
} StateRow;

typedef struct TransitionMatrix {
    StateRow states_rows[MAX_STATES];
    int width;
    int height;
} TransitionMatrix;

typedef struct SymbolVocab {
    char character;
    int column;
} SymbolVocab;

typedef struct AutomataDFA {
    SymbolVocab alphabet[MAX_ALPHABET_SIZE];        //
    int states[MAX_STATES];                         // [0,1,2,3] {could be just an int, but idk just in case}
    int start_state;                                // 1
    int current_state;                              // current_state (cs) changes when we read characters (cs == 1 and read w --> cs == 2)
    int accepting_states[MAX_STATES];               // [3] {could be more}
    TransitionMatrix matrix;                        // [[2,7,7,7,7],[7,3,7,7,7],[7,7,4,7,7],[7,7,7,5,7],[7,7,7,7,6],[7,7,7,7,7],[7,7,7,7,7]]
    Category type;                                  // CAT_KEYWORD (it is an enum, so include config.h to have the struct)
    bool dont_look_anymore;                         // If we have already finished the execution and we do not want to keep looking
} AutomataDFA;
//tmb hauria d tenir rules??

typedef struct AutomataList {
    AutomataDFA* automatas[MAX_AUTOMATAS];  
    int num_automata;
} AutomataList;

extern AutomataDFA* ALL_AUTOMATA[];
extern int NUM_AUTOMATA;

//------------------------------SRA-------------------------------------------------
typedef struct AutomataSRA {
    AutomataDFA* dfa;
    ParseTable table; 
    Stack* stack;
    int tokens; //Potser canviaria això per un contador (que conti per quin element de la llista anem) i que la llista sigui global
} AutomataSRA;

typedef struct LanguageV2 { // Language without Goto
    RuleItem terminals[MAX_ALPHABET_SIZE];
    int  num_terminals;
    RuleItem nonterminals[MAX_ALPHABET_SIZE][1]; //Podriem borrar el segon []
    int  num_nonterminals;
    RuleV2 productions[MAX_PRODUCTIONS];
    int  num_productions;
    char start_symbol[MAX_TOKEN_NAME];
    AutomataSRA* sra;
} LanguageV2;

//------------------Status------------------------------------------------- 
// CREC Q HAURIEM DE CANVIAR
typedef struct {
    Outformat oform;
	bool debug;
    bool help;

	char ifile_name[MAX_FILENAME]; 
	char ofile_name[MAX_FILENAME];
	FILE* ifile;
	FILE* ofile;
    FILE* error_file;

    ListTokens all_tokens;

	int line;           //In which line are we

    bool first_token_in_line ; //First token of the line
    bool line_has_tokens; //Si la línia té tokens (per no imprimir línies buides en RELEASE) 
} Status;
typedef struct BufferAuto {
    char lexeme[MAX_TOKEN_NAME];
    int len;
} BufferAuto;
extern Status status;



//------------------Crec q no necessitem-----------------------------------------------
// typedef struct StateRow {
//     int new_state[MAX_ALPHABET_SIZE]; // Contains a state_number for each state-char pair S0_row = [1,0,3,0,2] (assume symbols from the alphabet are always in the same order)
// } StateRow;

//Not sure if we will need these 2
// typedef struct ErrorReport {
//     Error error_type;            //Error type from the Error enum
//     Step step;                   //At which compilation step did it occur
//     int line;                    //Line number in source file
//     char message[MAX_LINE_LENGTH]; //Descriptive error message
// } ErrorReport;



// Path to the logs directory: put your full path, the directory has to exist
//#define PATHDIRLOGS "I:/Mi unidad/UPFdrive/docencia/github/compilers/modules_template/logs/" 
#define PATHDIRLOGS "./logs/" // For running yml

//----------------------- Function prototypes------------------------------------------
FILE* set_output_test_file(const char* filename);

const char* category_to_string(Category cat);

Category string_to_category(const char* str);

void add_token_to_list(char* lexeme, Category cat);

void buffer_clear(BufferAuto *buffer);

void buffer_add(BufferAuto *buffer, char c);

void buffer_append(BufferAuto *dest, const BufferAuto *src);

void buffer_move_append(BufferAuto *dest, BufferAuto *src);

void init_status_prs(void);

//Stack

void initialize_stack(Stack *stack, AutomataDFA dfa); //Before using a stack initialize it

void push_stack(Stack *stack, RuleItem symbol, int state); // push an element [rule symbol + state we are going to]

StackElement pop_stack(Stack *stack); //Pop stack

StackElement peek_stack(const Stack *stack); //Just see the top element, but not pop it

bool is_empty_stack(const Stack *stack);

bool is_full_stack(const Stack *stack);

void destroy_stack(Stack* stack);

// Helper functions for parser output
void stack_to_string(const Stack *stack, char *output, size_t output_size);

void action_to_string(ParseAction action, char *output, size_t output_size);

int load_language(const char* filename, LanguageV2* lang);

#endif // CONFIG_FILES_H