#include "debug_gen_test.h"

int main() {
    enum debug_info_enum_gen_test e = DEBUG_INFO_ENUM_GEN_TEST_B;
    struct debug_info_nested nested = {.x = 3.14, .e = DEBUG_INFO_ENUM_GEN_TEST_A};
    struct debug_info_gen_test test = {.a = 42, .b = 2.71f, .nested = &nested, .nested_val = {.x = 1.61, .e = DEBUG_INFO_ENUM_GEN_TEST_C}, .e_ptr = &e};

    MOE_PROC_MACRO_CALL(debug_info_gen_test, debug, &test);

    return 0;
}