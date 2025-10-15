#include "moe_string.h"

#include <string.h>


moe_string_t* moe_string_alloc(const char* data, int length) {
    moe_string_t* str = (moe_string_t*) MOE_ALLOC(moe_string_t);
    if (length == 0) {
        length = strlen(data);
    }
    int capacity = length + 1;// null terminator

    str->_internal = MOE_ALLOC(struct _moe_string_internal_s);
    str->_internal->data = MOE_ALLOC_RANGE(char, capacity);

    if (data != NULL) {
        memcpy(str->_internal->data, data, capacity);
        str->_internal->data[capacity - 1] = '\0';
    }

    str->length = length;
    str->capacity = capacity;

    str->_internal->ref_count = 1;

    return str;
}

moe_string_t* moe_string_clone(moe_string_t* str) {
    MOE_ASSERT(str != NULL);

    moe_string_t* dup = MOE_ALLOC(moe_string_t);
    dup->_internal = str->_internal;
    dup->length = str->length;
    dup->capacity = str->capacity;

    return dup;
}

char* moe_string_get(moe_string_t* str) {
    MOE_ASSERT(str != NULL);
    return str->_internal->data;
}

const char* moe_string_get_const(const moe_string_t* str) {
    MOE_ASSERT(str != NULL);
    return str->_internal->data;
}

void moe_string_free(moe_string_t* str) {
    if (str == NULL) {
        return;
    }

    str->_internal->ref_count -= 1;
    if (str->_internal->ref_count <= 0) {
        MOE_FREE(str->_internal->data);
        MOE_FREE(str->_internal);
    }

    MOE_FREE(str);
}

moe_bool moe_string_equal(const moe_string_t* a, const moe_string_t* b) {
    if (a == NULL || b == NULL) {
        return MOE_FALSE;
    }

    if (a->length != b->length) {
        return MOE_FALSE;
    }

    return strcmp(a->_internal->data, b->_internal->data) == 0
                   ? MOE_TRUE
                   : MOE_FALSE;
}
