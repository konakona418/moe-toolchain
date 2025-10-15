#include "debug_gen_test.h"
void _moe_proc_macro_debug_info_enum_gen_test_derive_debug_enum_impl(enum debug_info_enum_gen_test value) {
    switch(value) {
        case DEBUG_INFO_ENUM_GEN_TEST_A:
            printf("DEBUG_INFO_ENUM_GEN_TEST_A");
            break;
        case DEBUG_INFO_ENUM_GEN_TEST_B:
            printf("DEBUG_INFO_ENUM_GEN_TEST_B");
            break;
        case DEBUG_INFO_ENUM_GEN_TEST_C:
            printf("DEBUG_INFO_ENUM_GEN_TEST_C");
            break;
        default:
            printf("<unknown>");
            break;
    }
}
void _moe_proc_macro_debug_info_nested_derive_debug_impl_helper(struct debug_info_nested *self, int base_ident) {
    if (base_ident == 0) printf("%*sdebug_info_nested {\n", base_ident * 4, "");
    else printf("debug_info_nested {\n");
    printf("%*sx: double = %f\n", (base_ident + 1) * 4, "", self->x);
    printf("%*se: enum debug_info_enum_gen_test = ", (base_ident + 1) * 4, "");
    _moe_proc_macro_debug_info_enum_gen_test_derive_debug_enum_impl(self->e);
    printf("\n");
    printf("%*s}\n", base_ident * 4, "");
}
void _moe_proc_macro_debug_info_nested_derive_debug_impl(struct debug_info_nested *self) {
    _moe_proc_macro_debug_info_nested_derive_debug_impl_helper(self, 0);
}
void _moe_proc_macro_debug_info_gen_test_derive_debug_impl_helper(struct debug_info_gen_test *self, int base_ident) {
    if (base_ident == 0) printf("%*sdebug_info_gen_test {\n", base_ident * 4, "");
    else printf("debug_info_gen_test {\n");
    printf("%*sa: int = %d\n", (base_ident + 1) * 4, "", self->a);
    printf("%*sb: float = %f\n", (base_ident + 1) * 4, "", self->b);
    printf("%*snested: struct _* (%p) = ", (base_ident + 1) * 4, "", (void*)self->nested);
    if (self->nested) _moe_proc_macro_debug_info_nested_derive_debug_impl_helper(self->nested, base_ident + 1);
    printf("%*snested_val: struct _ = ", (base_ident + 1) * 4, "");
    _moe_proc_macro_debug_info_nested_derive_debug_impl_helper(&self->nested_val, base_ident + 1);
    printf("%*se_ptr: enum debug_info_enum_gen_test* (%p) = ", (base_ident + 1) * 4, "", (void*)self->e_ptr);
    if (self->e_ptr) _moe_proc_macro_debug_info_enum_gen_test_derive_debug_enum_impl(*self->e_ptr);
    printf("\n");
    printf("%*s}\n", base_ident * 4, "");
}
void _moe_proc_macro_debug_info_gen_test_derive_debug_impl(struct debug_info_gen_test *self) {
    _moe_proc_macro_debug_info_gen_test_derive_debug_impl_helper(self, 0);
}
