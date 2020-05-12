#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

#include "cf_threadpool/cf_threadpool.h"
#include "cf_allocator/cf_allocator_simple.h"
#include "cf_async_queue/cf_async_queue.h"
static struct cf_async_queue* aq = NULL;

static void test_consumer(void* d){
    while(true)
    {
        const char* str =  (char*)cf_async_queue_pop(aq);
        printf("recv str=%s\n",str);
    }
}

static void test_producer( void* d ){
    while(true)
    {
        cf_async_queue_push(aq,d);
        sleep(2);
    }
    
        
}

static void test_loop_print(void* d){
    for(int i =0;i < 10;i++){
        printf("i=%d\n",i);
        sleep(1);
    }
}


void test_async_queue(){
    aq = cf_async_queue_create(NULL);
    cf_async_queue_push(aq,(void*)123);
    cf_async_queue_push(aq,(void*)456);
    printf("%p\n",cf_async_queue_pop(aq));
    printf("%p\n",cf_async_queue_try_pop(aq));
    cf_async_queue_delete(aq);

    aq = cf_async_queue_create(NULL);
    cf_async_queue_set_max_size(aq,20);
    cf_async_queue_set_timeout(aq,900);
    cf_threadpool_run(test_loop_print,NULL);
    cf_threadpool_run(test_producer,(void*)"123");
    cf_threadpool_run(test_consumer,NULL);
    while(1)
    {
        sleep(1000);
    }
}