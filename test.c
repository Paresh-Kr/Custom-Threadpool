#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>

#include "ThreadPool.h"

pthread_mutex_t lock;
int rv,task;
int val_one=0;
int val_two=0;
int t_test=0;




void Run(void *args) {
    sleep(1);
    rv=pthread_mutex_lock(&lock);assert(rv == 0);
    val_one++;
   rv= pthread_mutex_unlock(&lock);assert(rv == 0);
}
void *run2(void *args) {
    sleep(10000);
    rv=pthread_mutex_lock(&lock);assert(rv == 0);
    val_two++;
    rv= pthread_mutex_unlock(&lock);assert(rv == 0);
}
void *run(void *vp) {
    t_test++;
    return 0;
}

int main()
{
   struct ThreadPool  t_pool;
   struct ThreadID *  t_id;
   puts("Creating Threadspool");
   int p,rv;
   void* tp;
    p  =ThreadPool_construct(&t_pool);
    sleep(10);
   tp =ThreadPool_run(&t_pool,t_id,&Run, NULL);
     if(tp ==0)
     {
            pthread_mutex_lock(&lock);
            task++;
            pthread_mutex_unlock(&lock);
      }

        fprintf(stderr, "No of task in ThreadPool_Run %d \n", task);

        fprintf(stderr, "From Run function task complete %d \n", val_one);
        ThreadPool_destruct(&t_pool);

        // test without threadpool
        int ec,i;


          while (1) {

                pthread_t tid1,tid2;
               for(i=0;i<20;i++){
                ec = pthread_create(&tid1, 0, run, 0);assert(ec == 0);
                rv = pthread_create(&tid2, 0, run2, 0);assert(rv == 0);

                rv = pthread_join(tid1, 0);assert(rv == 0);
                rv = pthread_join(tid2, 0);assert(rv == 0);

               }
                fprintf(stderr, "with Thread total task %d \n", t_test);

                 sleep(1);
                 exit(1);
            }

        return 0;

}
