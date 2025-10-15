#ifndef MOE_COMMON_LIST_H
#define MOE_COMMON_LIST_H

#include "moe_common.h"

#include <stddef.h>


typedef struct moe_list_node_s {
    struct moe_list_node_s* next;
    struct moe_list_node_s* prev;
} moe_list_node_t;

moe_list_node_t* moe_list_node_alloc();

void moe_list_node_init(moe_list_node_t* node);

void moe_list_node_free(moe_list_node_t* node);

void moe_list_add_head(moe_list_node_t* list, moe_list_node_t* node);

void moe_list_add_tail(moe_list_node_t* list, moe_list_node_t* node);

moe_bool moe_list_empty(moe_list_node_t* list);

moe_list_node_t* moe_list_head(moe_list_node_t* list);

moe_list_node_t* moe_list_tail(moe_list_node_t* list);

moe_list_node_t* moe_list_replace(moe_list_node_t* list, moe_list_node_t* node);

void moe_list_remove(moe_list_node_t* node);

typedef void (*moe__list_iterator_pfn)(moe_list_node_t* node, void* ctx);

typedef moe_bool (*moe__list_iterator_predicated_pfn)(moe_list_node_t* node, void* ctx);

#define MOE_PREDICATE_CONTINUE 1
#define MOE_PREDICATE_STOP 0

void moe_list_iterate(moe_list_node_t* head, moe__list_iterator_pfn iterator, void* ctx);

void moe_list_iterate_predicated(moe_list_node_t* head, moe__list_iterator_predicated_pfn iterator, void* ctx);

void moe_list_iterate_safe(moe_list_node_t* head, moe__list_iterator_pfn iterator, void* ctx);

void moe_list_reverse_iterate(moe_list_node_t* head, moe__list_iterator_pfn iterator, void* ctx);

void moe_list_reverse_iterate_predicated(moe_list_node_t* head, moe__list_iterator_predicated_pfn iterator, void* ctx);

#define moe_container_of(ptr, type, member)                \
    ({                                                     \
        const typeof(((type*) 0)->member)* __mptr = (ptr); \
        (type*) ((char*) __mptr - offsetof(type, member)); \
    })

#define moe_list_entry(node, type, member) \
    moe_container_of(node, type, member)

typedef struct moe_list_iterator_s {
    moe_list_node_t* current;
    moe_list_node_t* head;
} moe_list_iterator_t;

void moe_list_iterator_init(moe_list_iterator_t* iterator, moe_list_node_t* head);

moe_bool moe_list_iterator_next(moe_list_iterator_t* iterator);

moe_list_node_t* moe_list_iterator_get(moe_list_iterator_t* iterator);

#endif// MOE_COMMON_LIST_H