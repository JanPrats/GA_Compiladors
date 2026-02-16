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
 * Contributor/s: Pol García
 * -----------------------------------------------------------------------------
 */

#ifndef CONFIG_H
#define CONFIG_H
 
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>     // To set env for time zone
#include <string.h>     // For filename manipulation    
#include <time.h>       // For time-stamp of output logs filenames


//GLOBAL VARIABLES (GV)

//SINGULAR GV

//Configuration That defines how we compile (Change depending on what we want)
// "Flags"
#define DEBUG_F ON //On or OFF (defined below, I hope it is not a problem)
#define OUTFORMAT_M DEBUG //MODE of outformat (RELEASE or DEBUG)
#define COUNTOUT_F OUT //Explained below [OUT or DBGCOUNT] (in RELEASE Mode we not count)

#define PARSER_F false //Should not change until P3 (it will either continue with the parser or not) [IGNORE FOR NOW]

//Language
#define SPACE_CHAR ' '
#define TAB_CHAR '\t'
#define END_OF_LINE '\n'
#define CARRIAGE_RETURN '\r'

/* DEBUG_ FLAG      [For errors]
ON: (all) messages are written to the output file
OFF: (all) messages are written to the stdout
*/
/* COUNTOUT FLAG    [For counting]
OUT: messages should be sent to the output file
DBGCOUNT: messages should be sent to the stdout file.
*/

//These ones should not be changed (well, not usually to compile)
#define HELP_F "-help"
#define ERRORS_F "-errors"

/////"String" lengths
#define MAX_FILENAME 512        // Max File length (in bits I think) for compiler variables
#define MAX_TOKEN_NAME 4096      // Max Key Length
#define MAX_TOKENS 1024         // Max number of macros
#define MAX_LINE_LENGTH 4096    // Max length of a whole line
// #define MAX_MACRO_VALUE 1024    // MAX Value Length

#define MAX_FUNCTION_NAME 512
#define MAX_ALPHABET_SIZE 512
#define MAX_STATES 512
#define MAX_AUTOMATAS 256

#define MAXFILENAME 256 // Maximum length of the filename for output logs
#define MAXFILEEXT 64   // Maximum length of the file extension

//Return
#define ERROR_RETURN -1
#define CORRECT_RETURN 0
#define HELP_RETURN 1

#define ACCEPT_TOKEN 2
#define STOP_AUTOMATA 3

#define EOL_RETURN 4
#define EOF_RETURN 5


//Other
#define ON 1        //FOR DEBUG_F
#define OFF 0       //FOR DEBUG_F
#define OUT 1       //FOR COUNTOUT_F
#define DBGCOUNT 0  //FOR COUNTOUT_F



/////

//ENUMS

//For error Types when there is an error
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

//Depending on this we print in a format or in another in the outputfile
typedef enum{
    RELEASE,    // <lexeme, category> <lexeme, category> \n<lexeme, category> <lexeme, category> <lexeme, category>(if there is a \n in the file for now, maybe later it changes); NO EMPTY LINES
    DEBUG       // same as RELEASE but: empty line after every token line (\n\n for each \n i think); an line number before each non-empty line i.e 1 <lexeme, cat> <lexeme, cat> \n \n 3 <lexeme, cat> <lexeme, cat> (or 2 and not 3 not sure)             // The debugging messages should be written at the output file. So the messages are related with the tokens.
} Outformat;    // More modes can be added (and I think if DEBUG_F is true then outformat should be DEBUG)
// IF FORMAT == RELEASE ==> COUNTFIG = false
// IF FORMAT == DEBUG ==> COUNTFIG = true/false (depending on what we want, but usually true)

//Token Types
typedef enum{
    CAT_NUMBER,         //[0-9]+
    CAT_IDENTIFIER,     //[a-zA-Z] ([0-9] or [a-zA-Z])* {not repeated and not a keyword}
    CAT_KEYWORD,        //[a-zA-Z]+ (not sure if there can be numbers in keywords)
    CAT_TYPE,           //[a-zA-Z]+ (not sure if there can be numbers in keywords)
	CAT_LITERAL,        // " ASCII " (really any string between "")
	CAT_OPERATOR,       // = or > or + or *         ( i guess not - nor / nor < nor =< etcetc)
	CAT_SPECIALCHAR,    // ( or ) or ; or { or } or [ or ] or ,
	CAT_NONRECOGNIZED   // Does not fit in any of the previous
} Category;

typedef enum{ 
    SCANNER_STEP,
    PARSER_STEP
} Step;

// typedef enum{ //NOT SURE IF IT WILL BE NEEDED BE CAREFUL
//     FLAG_HELP,
// 	FLAG_DEBUG,
// 	FLAG_COUNTCONFIG,
// 	FLAG_PARSER //Whether we also call parser in main or just create the output file
// } Flag;


//Structs

typedef struct Token {
    char lexeme[MAX_TOKEN_NAME];    //Literal string (lexeme)    
    Category cat;                   //Category from the ones above
    int line;                       //Line number where the token starts
    // bool is_defined; //Not sure if needed
} Token;

// DICTIONARY NOT USED
// typedef struct IdendifierDict {
//     Token identifiers[MAX_TOKENS]; //Not sure if token or just a string (lexeme)
//     int count;                      //I will leave the option to add any relevant parameter such as count
// } IdendifierDict;

//List of tokens as thy appear in the ifile
typedef struct ListTokens {
    Token tokens[MAX_TOKENS];
    int count;
} ListTokens;

//This probably still needs things to be changed but for now we have this
//To count the input and output calls; The number of operations done etc etc
//Should create a "main" one and a new one each time we call a function and then add all the counters to the "main" counter once the function finishes

typedef struct CountVars {
    int countcomp;  //count number of comparisons being made (if ==> 2 comparisons)
    int countio;    //count the input/output instructions. Everytime we read or write we add 1 (to file or stdout)
    int countgen;   //count rest of instruction types
} CountVars;

typedef struct CountConfig {
    CountVars countvariables;
    bool countout;      //Defined above as a singular GV
    FILE* countifle;    //In case countout == DBGCOUNT we need an outputfile    <filename>.<ext>dbgcnt.
} CountConfig;

// Global struct used by all modules
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
	// bool last_in_line;  //Last Token of the line
    // bool in_string;
    // bool type_icv;      //Know if we are after an int, char or void declaration
} Status;

typedef struct StateRow {
    int new_state[MAX_ALPHABET_SIZE]; // Contains a state_number for each state-char pair S0_row = [1,0,3,0,2] (assume symbols from the alphabet are always in the same order)
} StateRow;
//???SHOULD BE [MAX_ALPHABET_SIZE] INSTEAD OF [MAX_STATES] ??????????????

typedef struct TransitionMatrix {
    StateRow states_rows[MAX_STATES]; // Contains a row for each state in the DFA [S0_row, S1_row, S2_row, ...]
    int width; //Number of symbols
    int height; //Number of states
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


typedef struct AutomataList {
    AutomataDFA* automatas[MAX_AUTOMATAS];  
    int num_automata;
} AutomataList;

extern AutomataDFA* ALL_AUTOMATA[];
extern int NUM_AUTOMATA;


/*

//DFA IF
alphabet:           ["i", "f"]      ==>  i = position_0_in_array ; f = position_1_in_array
states:             [1,2,3,4]       //4 == conjunt_buit state
start_state:        1
accepting_states:   [3]
la_acceptance:      ["(", " "]
TM:                 [[2,4],[4,3],[4,4],[4,4]]
Category:           CAT_KEYWORD

*/


//Not sure if we will need these 2
typedef struct ErrorReport {
    Error error_type;            //Error type from the Error enum
    Step step;                   //At which compilation step did it occur
    int line;                    //Line number in source file
    char message[MAX_LINE_LENGTH]; //Descriptive error message
} ErrorReport;

typedef struct CountReport {
    int line;
    char function_name[MAX_FUNCTION_NAME];
    int ammount; //ammount increased
    CountVars in_function_counts;
    CountVars overall_counts;
} CountReport;


//helper structs
typedef struct Two_ints {
    int actual;
    int lookahead;
} Two_ints;

typedef struct BufferAuto {
    char lexeme[MAX_TOKEN_NAME];
    int len;
} BufferAuto;

typedef struct ActionSkip {
    int c;
    int lookahead;
    int to_do;
} ActionSkip;

extern Status status;   // declaration, NOT definition

// Path to the logs directory: put your full path, the directory has to exist
//#define PATHDIRLOGS "I:/Mi unidad/UPFdrive/docencia/github/compilers/modules_template/logs/" 
#define PATHDIRLOGS "./logs/" // For running yml

// Function prototypes
FILE* set_output_test_file(const char* filename);

const char* category_to_string(Category cat);

void add_token_to_list(char* lexeme, Category cat);

void buffer_clear(BufferAuto *buffer);

void buffer_add(BufferAuto *buffer, char c);

void buffer_append(BufferAuto *dest, const BufferAuto *src);

void buffer_move_append(BufferAuto *dest, BufferAuto *src);

ActionSkip skip_nonchars(char c, char lookahead);

#endif // CONFIG_FILES_H