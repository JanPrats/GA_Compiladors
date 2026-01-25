/*
 * test_module_error.c
 * Test file for module_error.
 */

#include "../src/module_error/module_error.h"

int main() {
    report_error("This is a test error", 10);
    report_warning("This is a test warning", 20);
    return 0;
}
