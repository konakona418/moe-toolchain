#ifndef MOE_COMMON_ALLOC_H
#define MOE_COMMON_ALLOC_H

#define MOE_DEBUG
#define MOE_VERBOSE_ALLOC_MSG

#include <stdlib.h>

typedef struct moe__alloc_stats_s {
    size_t total_allocated;
    size_t total_freed;
} moe__alloc_stats_t;

moe__alloc_stats_t* moe_get_alloc_stats();

void moe_check_memory_leak();

void* moe_malloc(size_t size);

void* moe_calloc(size_t count, size_t size);

void moe_free(void* ptr);

#if defined(MOE_DEBUG) && defined(MOE_VERBOSE_ALLOC_MSG)

#include <stdio.h>

#define MOE_ALLOC(type)                                                   \
    ({                                                                    \
        void* ptr = moe_malloc(sizeof(type));                             \
        fprintf(stdout, "{\"action\":\"alloc\", \"type\":\"" #type "\", " \
                        "\"size\":%zu, \"addr\":\"%p\"}\n",               \
                sizeof(type), ptr);                                       \
        (type*) ptr;                                                      \
    })

#define MOE_ALLOC_RANGE(type, size)                                        \
    ({                                                                     \
        void* ptr = moe_calloc(size, sizeof(type));                        \
        fprintf(stdout, "{\"action\":\"calloc\", \"type\":\"" #type "\", " \
                        "\"size\":%zu, \"count\":%zu, \"addr\":\"%p\"}\n", \
                sizeof(type), (size_t) (size), ptr);                       \
        (type*) ptr;                                                       \
    })

#define MOE_FREE(ptr)                                                     \
    do {                                                                  \
        fprintf(stdout, "{\"action\":\"free\", \"addr\":\"%p\"}\n", ptr); \
        moe_free(ptr);                                                    \
    } while (0)

#else

#define MOE_ALLOC(type) ((type*) moe_malloc(sizeof(type)))
#define MOE_ALLOC_RANGE(type, count) ((type*) moe_malloc(sizeof(type) * (count)))
#define MOE_FREE(ptr) moe_free(ptr)

#endif// MOE_DEBUG && MOE_VERBOSE_ALLOC_MSG

#endif// MOE_COMMON_ALLOC_H