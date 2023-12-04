#include "pthreadpool.h"

struct task
{
    void (*function)(void *);
    void *arg;
};

struct pthreadpool
{
    size_t queue_size;
    struct task *queue;
    size_t head;
    size_t tail;
    size_t tasks;
    bool active;
    pthread_mutex_t mutex;
    pthread_cond_t write;
    pthread_cond_t read;
    pthread_t *threads;
    size_t thread_count;
};

char error_buffer[256];
#define SET_ERROR(message) (void)snprintf( \
    &error_buffer[0],                      \
    256,                                   \
    "%s:%d: %s: " message,                 \
    __FILE__,                              \
    __LINE__,                              \
    __FUNCTION__)

#define SET_ERRORF(format, ...) (void)snprintf( \
    &error_buffer[0],                           \
    256,                                        \
    "%s:%d: %s: " format,                       \
    __FILE__,                                   \
    __LINE__,                                   \
    __FUNCTION__,                               \
    __VA_ARGS__)

static inline size_t pthreadpool_next(pthreadpool_t threadpool, size_t index)
{
    return (index + 1) % threadpool->queue_size;
}

static void *pthreadpool_worker(void *arg)
{
    pthreadpool_t const threadpool = (pthreadpool_t)arg;
    struct task *current;
    while (threadpool->active)
    {
        current = NULL;
        (void)pthread_mutex_lock(&threadpool->mutex);
        while (threadpool->active && threadpool->tasks == 0)
        {
            (void)pthread_cond_wait(&threadpool->read, &threadpool->mutex);
        }
        if (threadpool->active)
        {
            current = threadpool->queue + threadpool->head;
            threadpool->tasks--;
            threadpool->head = pthreadpool_next(threadpool, threadpool->head);
        }
        pthread_mutex_unlock(&threadpool->mutex);
        if (current != NULL)
        {
            current->function(current->arg);
            pthread_cond_signal(&threadpool->write);
        }
    }
    return NULL;
}

int pthreadpool_create(
    pthreadpool_t *threadpool,
    size_t thread_count,
    size_t queue_size)
{
    int status;
    struct pthreadpool *new_threadpool;

    status = PTHREADPOOL_SUCCESS;
    new_threadpool = NULL;

    if (threadpool == NULL)
    {
        SET_ERROR("NULL threadpool handle");
        goto error;
    }

    if (thread_count == 0)
    {
        SET_ERRORF("invalid thread count: %lu", thread_count);
        goto error;
    }

    if (queue_size == 0)
    {
        SET_ERRORF("invalid queue size: %lu", thread_count);
        goto error;
    }

    status = PTHREADPOOL_SUCCESS;
    new_threadpool = calloc(1, sizeof(struct pthreadpool));
    new_threadpool->queue_size = queue_size;
    new_threadpool->queue = calloc(queue_size, sizeof(struct task));
    new_threadpool->head = 0;
    new_threadpool->tail = 0;
    new_threadpool->tasks = 0;

    new_threadpool->active = true;
    (void)pthread_mutex_init(&new_threadpool->mutex, NULL);
    (void)pthread_cond_init(&new_threadpool->read, NULL);
    (void)pthread_cond_init(&new_threadpool->write, NULL);
    new_threadpool->thread_count = thread_count;
    new_threadpool->threads = calloc(thread_count, sizeof(pthread_t));
    for (size_t thread_index = 0; thread_index < thread_count; thread_index++)
    {
        (void)pthread_create(
            new_threadpool->threads + thread_index,
            NULL,
            pthreadpool_worker,
            new_threadpool);
    }

    *threadpool = new_threadpool;
    goto done;
error:
    status = PTHREADPOOL_ERROR;
    if (new_threadpool != NULL)
    {
        pthreadpool_destroy(new_threadpool);
    }
done:
    return status;
}

int pthreadpool_destroy(
    pthreadpool_t threadpool)
{
    int status;

    status = PTHREADPOOL_SUCCESS;
    if (threadpool == NULL)
    {
        SET_ERROR("NULL threadpool");
        goto error;
    }

    if (threadpool->active)
    {
        (void)pthreadpool_shutdown(threadpool);
    }

    free(threadpool->threads);
    (void)pthread_cond_destroy(&threadpool->write);
    (void)pthread_cond_destroy(&threadpool->read);
    (void)pthread_mutex_destroy(&threadpool->mutex);
    free(threadpool->queue);
    free(threadpool);

    goto done;
error:
    status = PTHREADPOOL_ERROR;
done:
    return status;
}

int pthreadpool_submit(
    pthreadpool_t threadpool,
    void (*function)(void *), void *arg)
{
    int status;

    status = PTHREADPOOL_SUCCESS;
    if (threadpool == NULL)
    {
        SET_ERROR("NULL threadpool");
        goto error;
    }
    
    if (function == NULL)
    {
        SET_ERROR("NULL function");
        goto error;
    }

    (void)pthread_mutex_lock(&threadpool->mutex);
    while (threadpool->active && threadpool->tasks == threadpool->queue_size)
    {
        (void)pthread_cond_wait(&threadpool->write, &threadpool->mutex);
    }

    if (!threadpool->active)
    {
        SET_ERROR("failed to submit task to inactive threadpool");
        (void)pthread_mutex_unlock(&threadpool->mutex);
        goto error;
    }

    (threadpool->queue + threadpool->tail)->function = function;
    (threadpool->queue + threadpool->tail)->arg = arg;
    threadpool->tasks++;
    threadpool->tail = pthreadpool_next(threadpool, threadpool->tail);
    (void)pthread_mutex_unlock(&threadpool->mutex);
    (void)pthread_cond_signal(&threadpool->read);

    goto done;
error:
    status = PTHREADPOOL_ERROR;
done:
    return status;
}

int pthreadpool_shutdown(
    struct pthreadpool *threadpool)
{
    int status;

    status = PTHREADPOOL_SUCCESS;
    if (threadpool == NULL)
    {
        SET_ERROR("NULL threadpool");
        goto error;
    }

    if(!threadpool->active)
    {
        SET_ERROR("threadpool inactive");
        goto error;    
    }

    threadpool->active = false;
    (void)pthread_cond_broadcast(&threadpool->write);
    (void)pthread_cond_broadcast(&threadpool->read);
    for (size_t thread_index = 0; thread_index < threadpool->thread_count; thread_index++)
    {
        (void)pthread_join(
            *(threadpool->threads + thread_index),
            NULL);
    }

    goto done;
error:
    status = PTHREADPOOL_ERROR;
done:
    return status;
}

const char *pthreadpool_error()
{
    return &error_buffer[0];
}