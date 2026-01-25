#include <stdio.h>
#include "./module_ifdef_endif.h"
#include "../module_errors/module_errors.h"

void module_ifdef_endif_run(void) {
    printf("Loaded module_ifdef_endif: conditional compilation module\n");
}


static void ifdef_emit_placeholder_warning(void) {
    report_error(ERROR_WARNING, __FILE__, __LINE__, "module_ifdef_endif placeholder warning");
}
