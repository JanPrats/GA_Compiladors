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
#define COUNTCONFIG_F false //We count input/output, comparisons, instructions (in RELEASE Mode we should not count so false)
#define OUTFORMAT_M RELEASE_M //MODE of outformat (RELEASE_M or DEBUG_M)
#define COUNTOUT_F OUT //Explained below

#define PARSER_F false //Should not change until P3 (it will either continue with the parser or not) [IGNORE FOR NOW]

/* DEBUG_ FLAG      [For errors]
ON: (all) messages are written to the output file
OFF: (all) messages are written to the stdout
*/
/* COUNTOUT FLAG    [For counting]
OUT: messages should be sent to the output file
DBGCOUNT: messages should be sent to the stdout file.
*/

//These ones should not be changed (well, not usually to compile)
#define RELEASE_M Outformat RELEASE
#define DEBUG_M Outformat DEBUG
#define HELP_F "-help"

/////"String" lengths
#define MAX_FILENAME 512        // Max File length (in bits I think) for compiler variables
#define MAX_TOKEN_NAME 256      // Max Key Length
#define MAX_TOKENS 1024         // Max number of macros
#define MAX_LINE_LENGTH 4096    // Max length of a whole line
// #define MAX_MACRO_VALUE 1024    // MAX Value Length

#define MAX_FUNCTION_NAME 512
#define MAX_ALPHABET_SIZE 512
#define MAX_STATES 512

#define MAXFILENAME 256 // Maximum length of the filename for output logs
#define MAXFILEEXT 64   // Maximum length of the file extension

//Other
#define ON 1        //FOR DEBUG_F
#define OFF 0       //FOR DEBUG_F
#define OUT 1       //FOR COUNTOUT_F
#define DBGCOUNT 0  //FOR COUNTOUT_F

/////

//ENUMS

//For error Types when there is an error
typedef enum{
    INVALID_ARGUMENT //For now empty because idk which errors we consider and it is not my main focus now
} Error;

//Depending on this we print in a format or in another in the outputfile
typedef enum{
    RELEASE,    // <lexeme, category> <lexeme, category> \n<lexeme, category> <lexeme, category> <lexeme, category>(if there is a \n in the file for now, maybe later it changes); NO EMPTY LINES
    DEBUG       // same as RELEASE but: empty line after every token line (\n\n for each \n i think); an line number before each non-empty line i.e 1 <lexeme, cat> <lexeme, cat> \n \n 3 <lexeme, cat> <lexeme, cat> (or 2 and not 3 not sure)
                // The debugging messages should be written at the output file. So the messages are related with the tokens.
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

typedef enum{ //NOT SURE IF IT WILL BE NEEDED BE CAREFUL
    HELP,
	DEBUG,
	COUNTCONFIG,
	PARSER //Whether we also call parser in main or just create the output file
} Flag;


//Structs

typedef struct Token {
    char lexeme[MAX_TOKEN_NAME];    //Literal string (lexeme)    
    Category cat;                   //Category from the ones above
    // bool is_defined; //Not sure if needed
} Token;

// Dictionary oftokens to know there is no other token which name already used
typedef struct IdendifierDict {
    Token identifiers[MAX_TOKENS]; //Not sure if token or just a string (lexeme)
    int count;                      //I will leave the option to add any relevant parameter such as count
} IdendifierDict;

//List of tokens as thy appear in the ifile
typedef struct ListTokens {
    Token tokens[MAX_TOKENS];
    int count;
} IdendifierDict;

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
	bool countconfig;
	char ifile_name[MAX_MACRO_NAME]; 
	char ofile_name[MAX_MACRO_NAME];
	FILE* ifile;
	FILE* ofile;
    FILE* error_file;
	int line;           //In which line are we

    // bool first_in_line; //First token of the line
	// bool last_in_line;  //Last Token of the line
    // bool in_string;
    // bool type_icv;      //Know if we are after an int, char or void declaration
} Status;

typedef struct StateRow {
    int new_state[MAX_STATES] // Contains a state_number for each state-char pair S0_row = [1,0,3,0,2] (assume symbols from the alphabet are always in the same order)
} StateRow;

typedef struct TransitionMatrix {
    StateRow states_rows[MAX_STATES]; // Contains a row for each state in the DFA [S0_row, S1_row, S2_row, ...]
    int width; //Number of symbols
    int height; //Number of states
} TransitionMatrix;

typedef struct AutomataDFA {
    char alphablet[MAX_ALPHABET_SIZE];              // [w,h,i,l,e] {if we need w for transition matrix it is 0, for h it is 1, etc; as they appear} //Could also be a dictionary
    int states[MAX_STATES];                         // [1,2,3,4,5,6,7] {could be just an int, but idk just in case}
    int start_state;                                // 1
    int current_state;                              // current_state (cs) changes when we read characters (cs == 1 and read w --> cs == 2)
    int accepting_states[MAX_STATES];               // [6] {could be more}
    char lookahead_acceptance[MAX_ALPHABET_SIZE];   // ["("," ","\n", EOF]
    TransitionMatrix matrix;                        // [[2,7,7,7,7],[7,3,7,7,7],[7,7,4,7,7],[7,7,7,5,7],[7,7,7,7,6],[7,7,7,7,7],[7,7,7,7,7]]
    Category type;                                  // CAT_KEYWORD (it is an enum, so include config.h to have the struct)
    bool dont_look_anymore;                         // If we have already finished the execution and we do not want to keep looking
} AutomataDFA;

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
    Error cat;                   //Category from the ones above
    Step step;
    int line; //Maybe not needed
} ErrorReport;

typedef struct CountReport {
    int line;
    char function_name[MAX_FUNCTION_NAME];
    int ammount; //ammount increased
    CountVars in_function_counts;
    CountVars overall_counts;
} CountReport;

extern Status status;   // declaration, NOT definition

// Path to the logs directory: put your full path, the directory has to exist
//#define PATHDIRLOGS "I:/Mi unidad/UPFdrive/docencia/github/compilers/modules_template/logs/" 
#define PATHDIRLOGS "./logs/" // For running yml

// Function prototypes
FILE* set_output_test_file(const char* filename);
void status_init(
    Outformat oform,
    bool debug,
    bool countconfig,
    const char* ifile_name,
    const char* ofile_name,
    FILE* ifile,
    FILE* ofile,
    int line
);

#endif // CONFIG_FILES_H
