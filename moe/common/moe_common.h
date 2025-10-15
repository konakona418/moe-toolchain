#ifndef MOE_COMMON_COMMON_H
#define MOE_COMMON_COMMON_H

#include <assert.h>
#include <stdint.h>

#ifdef __cplusplus
#define MOE_IS_CPP
#endif

#define MOE_ASSERT(cond) (assert(cond))
#define MOE_ASSERT_MSG(cond, msg) (assert((cond) && (msg)))

typedef uint8_t moe_bool;
#define MOE_TRUE 1
#define MOE_FALSE 0

#ifdef MOE_IS_CPP
#define MOE_NULL nullptr
#else
#define MOE_NULL NULL
#endif

#define MOE_UNREACHABLE() MOE_ASSERT_MSG(0, "unreachable code reached")

#endif// MOE_COMMON_COMMON_H