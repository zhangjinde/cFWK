#include "cf_allocator/cf_allocator_simple.h"
#include "cf_async_queue/cf_async_queue.h"
#include <pthread.h>
struct _cf_thread_item
{
    pthread_t m_thread;
    pthread_mutex_t m_mutex;
    bool m_isbusy;
    void* m_data;
    struct cf_async_queue* m_queue;
    //CXASyncQueue<void (*)(void*)> m_queue;
};

static struct _cf_thread_item* create_thread_item(){
    
}


struct cf_threadpool{

};

struct cf_threadpool* cf_threadpool_create()
{
    return (struct cf_threadpool*)cf_allocator_simple_alloc(sizeof(struct cf_threadpool));
}
