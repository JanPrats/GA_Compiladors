#include "../config.h"

void init_status_scn(void);

void print_arguments(int argc, char *argv[]) {
    fprintf(status.ofile, "Arguments received (%d):\n", argc);
    for (int i = 0; i < argc; i++) {
        fprintf(status.ofile, "Argument %d: %s\n", i, argv[i]);
    }
    fflush(status.ofile);
}

void show_help(void) { // not finished
    printf("Flags you can use:\n");
    printf("  -help    Display this help message\n\n");
}


int init_program(int argc, char* argv[]){
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], HELP_F) == 0) { //If  we want to show the help manpage
            status.help = true;    // Global Variable for showing help and only showing help, no preprocessing
            return HELP_RETURN;
        } else if (argv[i][0] != '-') {
            strncpy(status.ifile_name, argv[i], MAX_FILENAME - 1);
        } // We assume if it is not "-"" it is not any flag but the input_file. In case this changes we would change this part
        else {
            char msg[256];
            snprintf(msg, sizeof(msg), "Unknown flag '%s' (ignored). Use -help for info.", argv[i]);
            //ERROR TO BE DONE
        }
    }
    init_status_scn();
    return CORRECT_RETURN;
}

void init_status_scn(){
    status.oform = RELEASE_M;
    status.debug = DEBUG_F;
    status.countconfig = COUNTCONFIG_F;

    snprintf(status.ofile_name, MAX_FILENAME, "%sscn", status.ifile_name);

    status.ifile_name[MAX_FILENAME - 1] = '\0';
    status.ofile_name[MAX_FILENAME - 1] = '\0';

    status.ifile = fopen(status.ifile_name, "r");
    if (!status.ifile) {
        fprintf(stderr, "CRITICAL ERROR: Cannot open input file '%s'\n", status.ifile_name);
        //add error
    }
    status.ofile = fopen(status.ofile_name, "w");
    if (!status.ofile) {
        fprintf(stderr, "CRITICAL ERROR: Cannot create output file '%s'\n", status.ofile_name);
        fclose(status.ifile);
        //add error
    }

    if(DEBUG_F == ON){
        status.error_file = stdout;
    }else if(DEBUG_F == OFF){
        status.error_file = status.ofile;
    }

    status.line = 0;
}

void init_automata(AutomataList* automata_list){
    automata_list->num_automata = NUM_AUTOMATA;
    for(int i = 0; i < NUM_AUTOMATA; i++){
        automata_list->automatas[i] = ALL_AUTOMATA[i];
    }
}

// FILE* set_output_test_file(const char* filename);