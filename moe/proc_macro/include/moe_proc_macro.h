#ifndef MOE_PROC_MACRO_H
#define MOE_PROC_MACRO_H

#define MOE_PROC_MACRO_ARG(type, name) type name

#define MOE_PROC_MACRO_DERIVES(struct_name, trait_name, ...) \
    struct struct_name;                                      \
    void _moe_proc_macro_##struct_name##_derive_##trait_name##_impl(struct struct_name* instance_ptr __VA_OPT__(, __VA_ARGS__));

#define MOE_PROC_MACRO_CALL(struct_name, trait_name, instance_ptr, ...) \
    _moe_proc_macro_##struct_name##_derive_##trait_name##_impl(instance_ptr __VA_OPT__(, __VA_ARGS__));

#endif// MOE_PROC_MACRO_H