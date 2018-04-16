#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#include "ThreadPool.h"

#define MaxThread 10
#define QueueSize 300

bool t_active=false;
bool t_idle=false;


int ThreadPool_construct(struct ThreadPool *tp){
    puts("Start :-ThreadPool_construct");
    //struct ThreadPool *t_pool=tp;
     int rv,i;

     {
         pthread_mutexattr_t attr;
         rv = pthread_mutexattr_init(&attr); assert(rv == 0);
         rv = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK); assert(rv == 0);
         rv = pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED); assert(rv == 0);
         rv = pthread_mutex_init(&tp->mutex, &attr); assert(rv == 0);
         rv = pthread_mutexattr_destroy(&attr); assert(rv == 0);
     }

     {
         pthread_condattr_t attr;
         rv = pthread_condattr_init(&attr); assert(rv == 0);
         rv = pthread_condattr_setpshared(&attr, PTHREAD_PROCESS_SHARED); assert(rv == 0);
         rv = pthread_cond_init(&tp->condvar, &attr); assert(rv == 0);
         rv = pthread_condattr_destroy(&attr); assert(rv == 0);
     }

    // rv = pthread_mutex_lock(&t_pool->mutex); assert(rv == 0);
     rv = pthread_mutex_lock(&tp->mutex); assert(rv == 0);
     t_active=true;

    if( (tp=(struct ThreadPool*)malloc(sizeof(struct ThreadPool)))== NULL)
    {
        puts("Failed :-ThreadPool_construct");

    }
     tp->list=(struct list * )malloc( sizeof(struct list));

     tp->thread_active=0;
     tp->thread_working=0;
     tp->thread_count=0;
     tp->Queue_Size= QueueSize;
     tp->list->x=0;
     tp->list->y=0;

     tp->thread_id = (struct ThreadID**)malloc( sizeof(struct ThreadID *) *MaxThread);
     tp->task_queue=(struct TaskQueue *)malloc(sizeof(struct TaskQueue) * QueueSize);

     for(i=0;i< MaxThread;i++)
     {
         int rv;
         rv = pthread_mutex_lock(&tp->mutex); assert(rv == 0);

         tp->thread_id[i] =(struct ThreadID*)malloc(sizeof(struct ThreadID));
         tp->thread_id[i]->threadpool= tp;
         tp->thread_id[i]->id= i;
         pthread_create(&(tp->thread_id[i])->thread, NULL, (void *)ThreadPool_join, (tp->thread_id[i])); // its give p_thread create error when passed argument ThreadID
         pthread_detach((tp->thread_id[i])->thread);
         tp->thread_count++;
         rv = pthread_mutex_unlock(&tp->mutex); assert(rv == 0);

       sleep(10);
     }
     rv = pthread_mutex_unlock(&tp->mutex); assert(rv == 0);

     puts("End :-ThreadPool_construct");
    return 0;
}

int ThreadPool_destruct(struct ThreadPool *t_pool){
    if(t_pool==NULL)
    {
      return 0;
    }
    int i,rv;
    rv = pthread_mutex_lock(&t_pool->mutex); assert(rv == 0);
    t_active=false;

    for(i=0;i< t_pool->thread_count;i++)
    {
       free(t_pool->thread_id[i]);
    }
    free(t_pool->thread_id);
    free(t_pool->task_queue);
    free(t_pool);
    pthread_mutex_destroy(&(t_pool->mutex));
    pthread_cond_destroy(&(t_pool->condvar));
    rv = pthread_mutex_unlock(&t_pool->mutex); assert(rv == 0);

 return 0;
}

void * ThreadPool_run(struct ThreadPool *tp, struct ThreadID *tid,void (*run)(void *), void *args){
    puts("start :-ThreadPool_run");
   // struct ThreadPool *t_pool=tp;
    int rv;
    rv = pthread_mutex_lock(&tp->mutex); assert(rv == 0);

    if(tp == NULL || run == NULL)
    {
        puts("Wrong :-ThreadPool Empty");
    }
      while(tp->thread_active){
               tp->task_queue[tp->list->y].run=run;
               tp->task_queue[tp->list->y].args=args;
               tp->list->y=(tp->list->y+1) % tp->Queue_Size;
               tp->thread_count++;
               (*(tp->task_queue[tp->list->y].run))(tp->task_queue[tp->list->y].args);
               tp->thread_active--;
                }
    rv = pthread_mutex_unlock(&tp->mutex); assert(rv == 0);


    puts("end :-ThreadPool_run");
    return 0;

}
int ThreadPool_join(struct ThreadID *tid, void **k)
{

   //struct ThreadPool* t_pool = tid.threadpool;
   struct TaskQueue task;
   int rv;
    if(t_active){
        puts("start :-ThreadPool_join");

        rv = pthread_mutex_lock(&tid->threadpool->mutex); assert(rv == 0);
        (tid->threadpool)->thread_working++;
        rv = pthread_mutex_unlock(&tid->threadpool->mutex); assert(rv == 0);

        task.run = (tid->threadpool)->task_queue[(tid->threadpool)->list->x].run;
        task.args = (tid->threadpool)->task_queue[(tid->threadpool)->list->x].args;
        (tid->threadpool)->list->x = ((tid->threadpool)->list->x +1) % (tid->threadpool)->Queue_Size;
        (tid->threadpool)->thread_count--;


        rv = pthread_mutex_lock(&(tid->threadpool)->mutex); assert(rv == 0);
        (tid->threadpool)->thread_working--;
        rv = pthread_mutex_unlock(&(tid->threadpool)->mutex); assert(rv == 0);

        puts("end :-ThreadPool_join");


    }

    return 0;

}
