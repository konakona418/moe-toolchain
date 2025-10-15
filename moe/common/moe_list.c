#include "moe_list.h"
#include "moe_alloc.h"


moe_list_node_t* moe_list_node_alloc() {
    moe_list_node_t* node = MOE_ALLOC(moe_list_node_t);
    moe_list_node_init(node);
    return node;
}

void moe_list_node_init(moe_list_node_t* node) {
    node->next = node;
    node->prev = node;
}

void moe_list_node_free(moe_list_node_t* node) {
    // MOE_ASSERT_MSG(node->next == NULL && node->prev == NULL, "node is not properly detached from list");

    MOE_FREE(node);
}

void moe_list_add_head(moe_list_node_t* list, moe_list_node_t* node) {
    MOE_ASSERT_MSG(list != NULL && node != NULL, "list or node is null");

    moe_list_node_t* orig_next = list->next;

    list->next = node;
    node->prev = list;

    node->next = orig_next;
    orig_next->prev = node;
}

void moe_list_add_tail(moe_list_node_t* list, moe_list_node_t* node) {
    MOE_ASSERT_MSG(list != NULL && node != NULL, "list or node is null");

    moe_list_node_t* orig_prev = list->prev;

    list->prev = node;
    node->prev = orig_prev;

    node->next = list;
    orig_prev->next = node;
}

moe_bool moe_list_empty(moe_list_node_t* list) {
    return list->next == list && list->prev == list;
}

moe_list_node_t* moe_list_head(moe_list_node_t* list) {
    return list->next;
}

moe_list_node_t* moe_list_tail(moe_list_node_t* list) {
    return list->prev;
}

moe_list_node_t* moe_list_replace(moe_list_node_t* list, moe_list_node_t* node) {
    MOE_ASSERT_MSG(list != NULL && node != NULL, "list or node is null");

    moe_list_node_t* replaced = list;

    list->prev->next = node;
    node->prev = list->prev;

    list->next->prev = node;
    node->next = list->next;

    replaced->next = NULL;
    replaced->prev = NULL;

    return replaced;
}

void moe_list_remove(moe_list_node_t* node) {
    MOE_ASSERT_MSG(node != NULL, "node is null");

    node->prev->next = node->next;
    node->next->prev = node->prev;

    node->next = NULL;
    node->prev = NULL;
}

void moe_list_iterate(moe_list_node_t* head, moe__list_iterator_pfn iterator, void* ctx) {
    moe_list_node_t* node = head->next;
    while (node != head) {
        iterator(node, ctx);
        node = node->next;
    };
}

void moe_list_iterate_predicated(moe_list_node_t* head, moe__list_iterator_predicated_pfn iterator, void* ctx) {
    moe_list_node_t* node = head->next;
    while (node != head) {
        moe_bool result = iterator(node, ctx);
        if (!result) {
            break;
        }
        node = node->next;
    };
}

void moe_list_iterate_safe(moe_list_node_t* head, moe__list_iterator_pfn iterator, void* ctx) {
    moe_list_node_t* node = head->next;
    while (node != head) {
        moe_list_node_t* next = node->next;
        iterator(node, ctx);
        node = next;
    };
}

void moe_list_reverse_iterate(moe_list_node_t* head, moe__list_iterator_pfn iterator, void* ctx) {
    moe_list_node_t* node = head->prev;
    while (node != head) {
        iterator(node, ctx);
        node = node->prev;
    }
}

void moe_list_reverse_iterate_predicated(moe_list_node_t* head, moe__list_iterator_predicated_pfn iterator, void* ctx) {
    moe_list_node_t* node = head->prev;
    while (node != head) {
        moe_bool result = iterator(node, ctx);
        if (!result) {
            break;
        }
        node = node->prev;
    }
}

void moe_list_iterator_init(moe_list_iterator_t* iterator, moe_list_node_t* head) {
    iterator->head = head;
    iterator->current = head;
}

moe_bool moe_list_iterator_next(moe_list_iterator_t* iterator) {
    iterator->current = iterator->current->next;
    if (iterator->current == iterator->head) {
        return MOE_FALSE;
    }

    return MOE_TRUE;
}

moe_list_node_t* moe_list_iterator_get(moe_list_iterator_t* iterator) {
    return iterator->current;
}
