#include "debug_gen_test.h"

int main() {
    struct debug_info_nested nested = {.x = 3.14};
    struct debug_info_gen_test test = {.a = 42, .b = 2.71f, .nested = &nested, .nested_val = {.x = 1.61}};

    MOE_PROC_MACRO_CALL(debug_info_gen_test, debug, &test, 0);

    return 0;
}