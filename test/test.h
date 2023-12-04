#include <pthreadpool.h>
#include <stdio.h>
#include <string.h>

#define TEST_PASS 0
#define TEST_FAIL 1

#define TEST_ASSERT(COND)   \
    if (!(COND))            \
    {                       \
        result = TEST_FAIL; \
        goto error;         \
    }