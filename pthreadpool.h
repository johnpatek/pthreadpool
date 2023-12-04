#ifndef _PTHREADPOOL_H_
#define _PTHREADPOOL_H_

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

enum
{
    PTHREADPOOL_SUCCESS,
    PTHREADPOOL_ERROR,
    PTHREADPOOL_TIMEOUT, // TODO: add timed submit functions
};

struct pthreadpool;

typedef struct pthreadpool *pthreadpool_t;

int pthreadpool_create(
    pthreadpool_t *threadpool,
    size_t thread_count,
    size_t queue_size);

int pthreadpool_destroy(
    pthreadpool_t threadpool);

int pthreadpool_submit(
    pthreadpool_t threadpool, 
    void (*function)(void *), void *arg);

int pthreadpool_shutdown(
    struct pthreadpool *threadpool);

const char *pthreadpool_error();

#endif