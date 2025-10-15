#include "debug_gen_test.h"
void _moe_proc_macro_debug_info_nested_derive_debug_impl(struct debug_info_nested *self, int base_ident) {
    printf("%*sdebug_info_nested {\n", base_ident * 4, "");
    printf("%*sx: double = %f\n", (base_ident + 1) * 4, "", self->x);
    printf("%*s}\n", base_ident * 4, "");
}
void _moe_proc_macro_debug_info_gen_test_derive_debug_impl(struct debug_info_gen_test *self, int base_ident) {
    printf("%*sdebug_info_gen_test {\n", base_ident * 4, "");
    printf("%*sa: int = %d\n", (base_ident + 1) * 4, "", self->a);
    printf("%*sb: float = %f\n", (base_ident + 1) * 4, "", self->b);
    printf("%*snested: struct debug_info_nested* (%p) = \n", (base_ident + 1) * 4, "", (void*)self->nested);
    if (self->nested) _moe_proc_macro_debug_info_nested_derive_debug_impl(self->nested, base_ident + 1);
    printf("%*snested_val: struct debug_info_nested = \n", (base_ident + 1) * 4, "");
    _moe_proc_macro_debug_info_nested_derive_debug_impl(&self->nested_val, base_ident + 1);
    printf("%*s}\n", base_ident * 4, "");
}
