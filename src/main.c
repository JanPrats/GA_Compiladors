#include "./main.h"
#include "./module_parser/module_parser.h"

FILE* ofile = NULL; // The output handler for the project run (same variable name as in modules)

int main(int argc, char *argv[]) {
    int n = 0;

    errors_init();

    ofile = stdout; // Default output to stdout
    ofile = set_output_test_file(PROJOUTFILENAME);

    fprintf(ofile, "Starting module args ...\n");
    
    ArgFlags* flags = process_arguments(argc, argv);
    if (!flags) { //Something wesnt wrong since module_args did not return the flags
        return 1;
    }

    if (flags->show_help) {
        show_help();
        free(flags);
        return 0; //End after showing manpage, do not preprocess anything
    }

    //Now we should start calling the parser here

    fprintf(ofile, "Starting module 2 ...\n");
    n = fib(FIBNUM);
    fprintf(ofile, "Fibonacci of %d is: %d\n", FIBNUM, n);
    fprintf(ofile, "Finished module 2!!\n");
    
    fprintf(ofile, "Starting module comments_remove ...\n");
    module_comments_run();
    fprintf(ofile, "Finished module comments_remove!!\n");
    
    fprintf(ofile, "Starting module define ...\n");
    module_define_run();
    fprintf(ofile, "Finished module define!!\n");
    
    fprintf(ofile, "Starting module errors ...\n");
    module_errors_run();
    fprintf(ofile, "Finished module errors!!\n");
    
    fprintf(ofile, "Starting module ifdef_endif ...\n");
    module_ifdef_endif_run();
    fprintf(ofile, "Finished module ifdef_endif!!\n");
    
    fprintf(ofile, "Starting module include ...\n");
    module_include_run();
    fprintf(ofile, "Finished module include!!\n");
    
    fprintf(ofile, "Starting module macros ...\n");
    module_macros_run();
    fprintf(ofile, "Finished module macros!!\n");
    
    printf("All modules executed successfully!\n\n");
    fprintf(ofile, "All modules executed successfully!\n\n");

    errors_finalize();

    if (errors_count() > 0) {
        return 1; // Devuelve error al sistema
    }

    fclose(ofile); 
    return 0;
}