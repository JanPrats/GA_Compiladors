/*
 * -----------------------------------------------------------------------------
 * module_args.c 
 *
 * This module provides functionality to process command-line arguments.
 * 
 * - `print_arguments`: Prints each command-line argument with its index.
 * - `process_arguments`: Intended to handle application-specific argument logic.
 *                        It sets all flags from call to the preprocessor (CLI args) 
 *                          and sets the input file name and the output file name.
 *
 * Usage:
 *     Called from the main application or test modules to process CLI args.
 *
 * Status:
 *     Implemented first version, may contain errors.
 *
 * Author: Pol García López
 * -----------------------------------------------------------------------------
 */

#include "./module_args.h"
#include "../module_parser/module_parser.h"
#include "../module_errors/module_errors.h"

void print_arguments(int argc, char *argv[]) {
    fprintf(ofile, "Arguments received (%d):\n", argc);
    for (int i = 0; i < argc; i++) {
        fprintf(ofile, "Argument %d: %s\n", i, argv[i]);
    }
    fflush(ofile);
}

void show_help(void) { // I still have to add more things
    printf("Flags you can use:\n");
    printf("  -c       Remove comments from source code (the default)\n");
    printf("  -d       Process directives (#include, #define, #ifdef, etc.)\n");
    printf("  -all     Enable all processing (comments + directives)\n");
    printf("  -help    Display this help message\n\n");
}

ArgFlags* process_arguments(int argc, char *argv[]) {
    ArgFlags* flags = (ArgFlags*)malloc(sizeof(ArgFlags));
    print_arguments(argc, argv);


    // Initialize the flags
    flags->comments_remove = false;
    flags->process_directives = false;
    flags->show_help = false;
    flags->ifile[0] = '\0'; //"empty" string
    flags->ofile[0] = '\0';
    char* input_filename = NULL; // File name (as a string)

    // Itentify each flag
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-c") == 0) { // If the flag is -c
            flags->comments_remove = true; // Global Variable for removing comments is now true
        } else if (strcmp(argv[i], "-d") == 0) { //If flas is -d
            flags->process_directives = true; // Same but for directives
        } else if (strcmp(argv[i], "-all") == 0) { // equal to -c -d
            flags->comments_remove = true;
            flags->process_directives = true; //2 previous steps in 1
        } else if (strcmp(argv[i], "-help") == 0) { //If on the other hand we want to show the help manpage
            flags->show_help = true;    // Global Variable for showing help and only showing help, no preprocessing
            return flags;
        } else if (argv[i][0] != '-') {
            input_filename = argv[i];} // We assume if it is not "-"" it is not any flag but the input_file. In case this changes we would change this part
        else {
            char msg[256];
            snprintf(msg, sizeof(msg), "Unknown flag '%s' (ignored). Use -help for info.", argv[i]);
            report_error(ERROR_WARNING, __FILE__, __LINE__, msg);
        }
    }

    if (!flags->comments_remove && !flags->process_directives) { // In case there was not a -c -d or -all flag, we will use the default (-c)
        flags->comments_remove = true; // -c
    }

    strncpy(flags->ifile, input_filename, MAX_FILENAME - 1); //Copy input file name to ifile entry of the struct
    flags->ifile[MAX_FILENAME - 1] = '\0'; // Last character has to be \0 to identify it is a string and not a list of characters

    // Now to generate output filename: {input_basename}_pp.c
    char* input_copy = strdup(input_filename); //We duplicate the string just in case we ned the original later
    char* base = basename(input_copy); //basename gets teh "base" filename from the whole file direction (from ./whatever/src/input-example.c to just input-example.c)
    
    // we need to remove ".c" first
    char* dot = strrchr(base, '.'); // Create a pointer towards the character "." inside the string called base
    if (dot) {      // If there is a character in dot. In other words, if there is a "." inside base
        *dot = '\0';    //Now that "." becomes a \0 meaning we go [from input-example.c to input-example\0c]. Since strings "finish" at \0 we have "cut" the original filename
    }
    
    //snprintf prints base into flags->ofile using format "%s_pp.c", not much of a mystery
    snprintf(flags->ofile, MAX_FILENAME, "%s_pp.c", base); // We add _pp.c to the original filename (it reads until \0 so it only reads input-example and after adding the _pp.c it becomes input-example_pp.c)
    free(input_copy); //We don't need it

    // fprintf(ofile, "Module arguments: not implemented yet\n");
    // fflush(ofile);
    return flags;
}
