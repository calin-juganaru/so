#include <stdio.h>
#include <stdlib.h>

#include "so_scheduler.h"

// ============================================================================

typedef enum
{
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED
} STATE;

// ============================================================================

typedef struct
{
    tid_t id;
    int priority;
    STATE state;
    so_handler* func;
} so_thread;

so_thread* current_thread;

// ============================================================================

typedef struct
{
    size_t size, capacity;
    so_thread* data;
} vector;

vector threads;

// ============================================================================

struct { int quanta, ios, time; } so_scheduler;

// ============================================================================

static void increment_time()
{
    so_scheduler.time++;
    if (so_scheduler.time == so_scheduler.quanta)
    {
        so_scheduler.time = 0;

    }
}

// ============================================================================

void* thread_function(void* arg)
{
    int index = *(int*)arg;

    threads.data[index].state = RUNNING;
    current_thread = &threads.data[index];
    threads.data[index].func(threads.data[index].priority);
    threads.data[index].state = TERMINATED;
    return NULL;
}

// ============================================================================

static tid_t push_back(so_handler* func, unsigned priority)
{
    so_thread thread;
    thread.func = func;
    thread.priority = priority;
    thread.state = NEW;

    if (threads.size == threads.capacity)
    {
        threads.capacity *= 2;
        threads.data = realloc(threads.data, threads.capacity);
    }

    int index = threads.size;
    ++threads.size;
    threads.data[index] = thread;

    int retval = pthread_create(&threads.data[index].id, NULL,
                                thread_function, (void*)&index);
    if (retval < 0) return INVALID_TID;

    return threads.data[index].id;
}

// ============================================================================

DECL_PREFIX int so_init(unsigned int time_quantum, unsigned int io)
{
    if (time_quantum < 1 || io < 0 ||
        io > SO_MAX_NUM_EVENTS ||
        threads.data != NULL)
    {
        so_end();
        return -1;
    }

    so_scheduler.quanta = time_quantum;
    so_scheduler.ios = io;

    threads.data = calloc(1, sizeof(so_thread));
    threads.size = 0;
    threads.capacity = 1;

    return 0;
}

// ============================================================================

DECL_PREFIX void so_end(void)
{
    if (threads.data != NULL)
    {
        for (int i = 0; i < threads.size; ++i)
		    pthread_join(threads.data[i].id, NULL);

        free(threads.data);
        threads.data = NULL;
        current_thread = NULL;
    }
}

// ============================================================================

DECL_PREFIX void so_exec(void)
{
    exit(0);
}

// ============================================================================

DECL_PREFIX int so_wait(unsigned int io)
{
    return 0;
}

// ============================================================================

DECL_PREFIX int so_signal(unsigned int io)
{
    return 0;
}

// ============================================================================

DECL_PREFIX tid_t so_fork(so_handler* func, unsigned int priority)
{
    if (priority < 0 || priority > SO_MAX_PRIO || func == NULL)
        return INVALID_TID;

    tid_t thread_id = push_back(func, priority);

    return thread_id;
}

// ============================================================================