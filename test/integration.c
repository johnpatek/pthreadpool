#include "test.h"

#include <time.h>

// integration test
static int test_pthreadpool();

int main(int argc, char ** argv)
{
    int status;
    status = test_pthreadpool();
    
    return status;
}

int test_pthreadpool()
{
    return 0;
}