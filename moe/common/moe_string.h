#ifndef MOE_STRING_H
#define MOE_STRING_H

#include <moe_alloc.h>
#include <moe_common.h>

typedef struct moe_string_s {
    struct _moe_string_internal_s {
        char* data;
        int ref_count;
    }* _internal;

    int length;
    int capacity;
} moe_string_t;

moe_string_t* moe_string_alloc(const char* data, int length);

moe_string_t* moe_string_clone(moe_string_t* str);

char* moe_string_get(moe_string_t* str);

const char* moe_string_get_const(const moe_string_t* str);

void moe_string_free(moe_string_t* str);

moe_bool moe_string_equal(const moe_string_t* a, const moe_string_t* b);

#endif// MOE_STRING_H