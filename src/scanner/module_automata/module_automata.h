#ifndef MODULE_AUTOMATA_H
#define MODULE_AUTOMATA_H
#include <stdbool.h>
#include "config.h"





bool automata_is_accepting(const AutomataDFA* a, int st);

void automata_driver(FILE* input_file, FILE* output_file, AutomataDFA* automata_list, int num_automata);

 #endif
