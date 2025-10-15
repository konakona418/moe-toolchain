#include "moe_alloc.h"
#include "moe_common.h"

moe__alloc_stats_t* moe_get_alloc_stats() {
    static moe__alloc_stats_t stats = {0, 0};
    return &stats;
}

void moe_check_memory_leak() {
    moe__alloc_stats_t* stats = moe_get_alloc_stats();
    MOE_ASSERT_MSG(stats->total_allocated == stats->total_freed, "memory leak detected");
}

void* moe_malloc(size_t size) {
    moe_get_alloc_stats()->total_allocated += 1;
    return malloc(size);
}

void* moe_calloc(size_t count, size_t size) {
    moe_get_alloc_stats()->total_allocated += 1;
    return calloc(count, size);
}

void moe_free(void* ptr) {
    moe_get_alloc_stats()->total_freed += 1;
    free(ptr);
}