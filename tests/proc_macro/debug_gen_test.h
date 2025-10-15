#include <moe_proc_macro.h>

#include <stdio.h>

MOE_PROC_MACRO_DERIVES(
        debug_info_nested, debug,
        MOE_PROC_MACRO_ARG(int, base_ident))
struct debug_info_nested {
    double x;
};

MOE_PROC_MACRO_DERIVES(
        debug_info_gen_test, debug,
        MOE_PROC_MACRO_ARG(int, base_ident))
struct debug_info_gen_test {
    int a;
    float b;
    struct debug_info_nested* nested;
    struct debug_info_nested nested_val;
};