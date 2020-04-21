#include "cf_allocator/cf_allocator_simple.h"
#include "cf_async_queue.h"
#include "cf_collection/cf_list.h"
#include <pthread.h>
struct cf_async_queue{
    pthread_mutex_t m_mutex;
    pthread_cond_t  m_condition;
    struct cf_list* m_queue;

};
struct cf_async_queue* cf_async_queue_create(void (*free_data)(void*)){
    struct cf_async_queue* queue = cf_allocator_simple_alloc(sizeof(struct cf_async_queue));
    if(queue)
    {
        pthread_mutex_init(&queue->m_mutex,NULL); 
        pthread_cond_init(&queue->m_condition,NULL);
        struct cf_list* list = cf_list_create(free_data);
        if(list)
        {
            queue->m_queue = list;
        }
        else
        {
            cf_allocator_simple_free(queue);
            queue = NULL;
        }
    }
    return queue;
}
void* cf_async_queue_pop(struct cf_async_queue* queue)
{
    pthread_mutex_lock(&queue->m_mutex);
    if(cf_list_is_empty(queue->m_queue) == true){
            pthread_cond_wait(&queue->m_condition,&queue->m_mutex);
    }
    void* item = cf_list_take_front(queue->m_queue);
    pthread_mutex_unlock(&queue->m_mutex);
    return item;
}

void cf_async_queue_push(struct cf_async_queue* queue,void* item){
    pthread_mutex_lock(&queue->m_mutex);
    cf_list_push(queue->m_queue,item);
    if(cf_list_length(queue->m_queue) == 1)
    {
        pthread_cond_signal(&queue->m_condition);
    }
    pthread_mutex_unlock(&queue->m_mutex);
}
void cf_async_queue_delete(struct cf_async_queue* queue){
    pthread_mutex_destroy(&queue->m_mutex);
    pthread_cond_destroy(&queue->m_condition);
    cf_list_delete(queue->m_queue);
    cf_allocator_simple_free(queue);
}