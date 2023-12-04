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

static int pthreadpool_test_create();
static int pthreadpool_test_destroy();
static int pthreadpool_test_submit();
static int pthreadpool_test_shutdown();

static void pthreadpool_test_function(void *arg);

int main(int argc, char **argv)
{
    int status;
    status = 0;
    if (argc > 1)
    {
        if (strcmp(argv[1], "create") == 0)
        {
            status = pthreadpool_test_create();
        }
        else if (strcmp(argv[1], "destroy") == 0)
        {
            status = pthreadpool_test_destroy();
        }
        else if (strcmp(argv[1], "submit") == 0)
        {
            status = pthreadpool_test_submit();
        }
        else if (strcmp(argv[1], "shutdown") == 0)
        {
        }
        else
        {
            printf("main: invalid test name \"%s\"\n", argv[1]);
        }
    }
    return status;
}

int pthreadpool_test_create()
{
    pthreadpool_t threadpool;
    int result;

    result = TEST_PASS;

    TEST_ASSERT(pthreadpool_create(NULL, 1, 1) == PTHREADPOOL_ERROR)
    TEST_ASSERT(pthreadpool_create(&threadpool, 0, 1) == PTHREADPOOL_ERROR)
    TEST_ASSERT(pthreadpool_create(&threadpool, 1, 0) == PTHREADPOOL_ERROR)
    TEST_ASSERT(pthreadpool_create(&threadpool, 1, 1) == PTHREADPOOL_SUCCESS)

    (void)pthreadpool_destroy(threadpool);

    goto done;
error:
    result = 1;
done:
    return result;
}

int pthreadpool_test_destroy()
{
    pthreadpool_t threadpool;
    int result;

    result = TEST_PASS;

    TEST_ASSERT(pthreadpool_create(&threadpool, 1, 1) == PTHREADPOOL_SUCCESS)
    TEST_ASSERT(pthreadpool_destroy(NULL) == PTHREADPOOL_ERROR)
    TEST_ASSERT(pthreadpool_destroy(threadpool) == PTHREADPOOL_SUCCESS)

    goto done;
error:
    result = 1;
done:
    return result;
}

int pthreadpool_test_submit()
{
    pthreadpool_t threadpool;
    int result;

    result = TEST_PASS;

    TEST_ASSERT(pthreadpool_create(&threadpool, 4, 4) == PTHREADPOOL_SUCCESS)
    TEST_ASSERT(pthreadpool_submit(NULL, pthreadpool_test_function, NULL) == PTHREADPOOL_ERROR)
    TEST_ASSERT(pthreadpool_submit(threadpool, NULL, NULL) == PTHREADPOOL_ERROR)
    TEST_ASSERT(pthreadpool_submit(threadpool, pthreadpool_test_function, NULL) == PTHREADPOOL_SUCCESS)
    //TEST_ASSERT(pthreadpool_shutdown(threadpool) == PTHREADPOOL_SUCCESS)
    //TEST_ASSERT(pthreadpool_submit(threadpool, pthreadpool_test_function, NULL) == PTHREADPOOL_ERROR)
    TEST_ASSERT(pthreadpool_destroy(threadpool) == PTHREADPOOL_SUCCESS)

    goto done;
error:
    result = 1;
done:
    return result;
}

int pthreadpool_test_shutdown()
{
    return 0;
}

static void pthreadpool_test_function(void *arg)
{
    // this function does nothing
}