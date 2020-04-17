#include "cf_allocator/cf_allocator_simple.h"
#include "cf_async_queue/cf_async_queue.h"
#include <pthread.h>
struct _thread_item
{
    pthread_t m_thread;
    pthread_mutex_t m_mutex;
    bool m_isbusy;
    void* m_data;
    struct cf_async_queue* m_queue;
    //CXASyncQueue<void (*)(void*)> m_queue;
};

static void* _thread_item_run(void* arg){
    struct _thread_item *_this = (struct _thread_item*)arg;
    
    while(true){
        void (*func)(void*) = cf_async_queue_pop(_this->m_queue);
        func(_this->m_data);
        _this->m_data = NULL;
        pthread_mutex_lock(&_this->m_mutex);
    }
    return NULL;
}

static bool _thread_item_start(struct _thread_item* item,void (*run)(void*),void* d){
    int ret = pthread_mutex_trylock(&item->m_mutex);
    if(ret == 0){
        item->m_data = d;
        cf_async_queue_push(item->m_queue,run);
    }
    return ret == 0 ? true : false;
}

static struct _thread_item* create_thread_item(){
    struct _thread_item* item = (struct _thread_item*)cf_allocator_simple_alloc(sizeof(struct _thread_item));
    if(item)
    {
        item->m_isbusy = false;
        item->m_queue = cf_async_queue_create();
        pthread_create(&item->m_thread,0,_thread_item_run,item);
        pthread_mutex_init(&item->m_mutex,NULL);
    }
    return item;
}

static void _thread_delete(struct _thread_item* item)
{
    pthread_cancel(item->m_thread);
    pthread_mutex_destroy(&item->m_mutex);
    cf_async_queue_delete(item->m_queue);
};

struct cf_threadpool{
    size_t m_max_thread_count ;
    struct cf_list* m_threads;
};

struct cf_threadpool* cf_threadpool_create(size_t thread_count)
{
    struct cf_threadpool* threadpool =  (struct cf_threadpool*)cf_allocator_simple_alloc(sizeof(struct cf_threadpool));
    if(threadpool)
    {
        threadpool->m_max_thread_count = thread_count;
        threadpool->m_threads = cf_list_create();
        for(size_t i = 0;i < thread_count;i++)
        {
            cf_list_push(threadpool->m_threads,create_thread_item());
        }
    }
    return threadpool;
}
int cf_threadpool_start(struct cf_threadpool* threadpool,void (*run)(void*),void* d){
    for(struct cf_iterator iter = cf_list_begin(threadpool->m_threads);!cf_iterator_is_end(&iter);cf_iterator_next(&iter))
    {
        struct  _thread_item*  item = cf_iterator_get(&iter);
        
        if(_thread_item_start(item,run,d) == true)
            return 0;
    }
    return -1;
}
static struct cf_threadpool* cf_threadpool_global_instance(){
    static struct cf_threadpool* instance = NULL;
    if(instance == NULL){
        instance = cf_threadpool_create(40);
    }
    return instance;
}
int cf_threadpool_run(void (*run)(void*),void* d){
    struct cf_threadpool* threadpool = cf_threadpool_global_instance();
    return cf_threadpool_start(threadpool,run,d);
}
/*************************************
 * gcc -DCF_THREADPOOL_TEST -g -I../ cf_threadpool.c ../cf_allocator/cf_allocator_simple.c ../cf_async_queue/cf_async_queue.c ../cf_collection/cf_list.c ../cf_collection/cf_iterator.c -lpthread -o cf_threadpool_test
 * ***********************************/
#ifdef CF_THREADPOOL_TEST
#include <unistd.h>
#include <stdio.h>
static struct cf_async_queue* aq = NULL;

static void test_consumer(void* d){
    while(true)
    {
        const char* str =  cf_async_queue_pop(aq);
        printf("recv str=%s\n",str);
        sleep(1);
    }
}

static void test_producer( void* d ){
    while(true)
    {
        cf_async_queue_push(aq,"hell");
        sleep(1);
    }
    
        
}

static void test_loop_print(void* d){
    for(int i =0;i < 10;i++){
        printf("i=%d\n",i);
        sleep(1);
    }
}

void util_test(){
    aq = cf_async_queue_create();
    cf_threadpool_run(test_loop_print,NULL);
    cf_threadpool_run(test_producer,NULL);
    cf_threadpool_run(test_consumer,NULL);
}
int main(){
    util_test();
    while(1){
        sleep(1);
    }
}
#endif//CF_THREADPOOL_TEST