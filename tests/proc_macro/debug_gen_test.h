#include <moe_proc_macro.h>

#include <stdio.h>

MOE_PROC_MACRO_DERIVES_ENUM(debug_info_enum_gen_test, debug_enum)
enum debug_info_enum_gen_test {
    DEBUG_INFO_ENUM_GEN_TEST_A = 1,
    DEBUG_INFO_ENUM_GEN_TEST_B = 2,
    DEBUG_INFO_ENUM_GEN_TEST_C = 3,
};

MOE_PROC_MACRO_DERIVES(debug_info_nested, debug)
struct debug_info_nested {
    double x;
    enum debug_info_enum_gen_test e;
};

MOE_PROC_MACRO_DERIVES(debug_info_gen_test, debug)
struct debug_info_gen_test {
    int a;
    float b;
    struct debug_info_nested* nested;
    struct debug_info_nested nested_val;
    enum debug_info_enum_gen_test* e_ptr;
};