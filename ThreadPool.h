#ifndef THREADPOOL_H
#define THREADPOOL_H

#endif // THREADPOOL_H
#include <unistd.h>
#include <stddef.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>


typedef struct TaskQueue{
    int queue_count;
    char buf[100];
    int variable;
    void (*run)(void *);
    void *args;
}taskqueue;

typedef struct list {
  int x;
  int y;
  int count;
  struct list *next;
}l_list;

typedef struct ThreadID{     // Define the contents of this struct in your header file.
 int id;
 pthread_t  thread;
 struct ThreadPool* threadpool;
}threadid;
struct ThreadPool{      // Define the contents of this struct in your header file.
    pthread_mutex_t mutex;
    pthread_cond_t condvar;
    int thread_count;
    int thread_active;
    int thread_working;
    int Queue_Size;
    threadid** thread_id;
    taskqueue* task_queue;
    l_list* list;
};
int ThreadPool_construct(struct ThreadPool *);
int ThreadPool_destruct(struct ThreadPool *);
void * ThreadPool_run(struct ThreadPool *, struct ThreadID *, void (*run)(void *), void *);
int ThreadPool_join(threadid *, void **);
