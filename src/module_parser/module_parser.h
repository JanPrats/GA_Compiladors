#ifndef MODULE_PARSER_H
#define MODULE_PARSER_H

#include "../main.h" //Is this one necessary?
#include <stdbool.h>
#include <stdio.h>

// Global Variables
#define MAX_FILENAME 512        // Max File length (in bits I think)
#define MAX_MACRO_NAME 256      // Max Key Length
#define MAX_MACRO_VALUE 1024    // MAX Value Length
#define MAX_MACROS 1024         // Max number of macros

// Macro dictionary entry (Name of the Macro, its value and if it is defined or not)
typedef struct MacroEntry {
    char name[MAX_MACRO_NAME];      
    char value[MAX_MACRO_VALUE];
    bool is_defined;
} MacroEntry;

// Macro dictionary (dictionary where all the macros will be stored at with a counter of how many Macros there are)
typedef struct MacroDict {
    MacroEntry entries[MAX_MACROS];
    int count;
} MacroDict;

// Parser state structure
typedef struct ParserState {
    FILE* current_file; //The file we are reading (input file)
    FILE* output_file; // The file we are writing at (output file)
    char current_filename[MAX_FILENAME]; //The name of the file we are reading (input file)
    int current_line; //The name of the file we are writing at (output file)
    MacroDict* macro_dict; // Dictionary of Macros
    bool comments_remove;  // -c 
    bool process_directives; // -d 
    bool in_string;  // To know if we are inside a string so the parser does not interpret directives or macros inside strings [For example: printf("MAX_MACROS") should not be substituted to printf("1024")]
    bool in_comment; // To know if we are inside a comment so the parser does not interpret directives or macros inside comments
    char lookahead;  // For lookahead parsing (It should contain the next character to be read without parser actually reading it. We use it in peek_char, we'll use it to now if we want to read the following character or not)
    bool has_lookahead; //To now if we have "read" the next character without actually having read it 
} ParserState;

// Flags from command-line arguments -c -d -all -help
typedef struct ArgFlags {
    bool comments_remove; // -c (default)
    bool process_directives; // -d
    bool show_help; // -help
    char ifile[MAX_FILENAME]; //input file name as a string
    char ofile[MAX_FILENAME]; //output file name as a string
} ArgFlags;

// Parser initialization and cleanup
ParserState* init_parser(const char* input_file, const char* output_file, ArgFlags* flags);
void cleanup_parser(ParserState* state);

// Main parsing function
int parse_until(ParserState* state, const char* stop_symbol, bool copy_to_output);

// Helper functions for character/word reading
char read_char(ParserState* state);
char peek_char(ParserState* state);
void unread_char(ParserState* state, char c);
void skip_whitespace(ParserState* state);
bool is_whitespace(char c);
bool is_identifier_char(char c);


#endif // MODULE_PARSER_H

