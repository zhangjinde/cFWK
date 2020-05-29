#include "cf_allocator/cf_allocator_simple.h"
#include "cf_async_queue.h"
#include "cf_collection/cf_list.h"
#include <pthread.h>

 #include <sys/time.h>
#include "cf_logger/cf_logger.h"
typedef struct cf_async_queue{
    pthread_mutex_t m_mutex;
    pthread_cond_t  m_condition;
    struct cf_list* m_queue;
    int m_max_size;
    uint32_t m_timeout; //ms

}cf_async_queue;
void cf_async_queue_set_max_size( cf_async_queue* queue,int max_size){
    queue->m_max_size = max_size;
}
void cf_async_queue_set_timeout(struct cf_async_queue* queue,uint32_t timeout){
    queue->m_timeout = timeout;
}
struct cf_async_queue* cf_async_queue_create(void (*free_data)(void*)){
    struct cf_async_queue* queue = (struct cf_async_queue*)cf_allocator_simple_alloc(sizeof(struct cf_async_queue));
    if(queue)
    {
        pthread_mutex_init(&queue->m_mutex,NULL); 
        pthread_cond_init(&queue->m_condition,NULL);
        queue->m_timeout = 0;
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
int cf_async_queue_length(struct cf_async_queue* queue){
    pthread_mutex_lock(&queue->m_mutex);
    int len = cf_list_length(queue->m_queue); 
    pthread_mutex_unlock(&queue->m_mutex);
    return len;
}
void* cf_async_queue_pop(struct cf_async_queue* queue)
{
    void* item = NULL;
    pthread_mutex_lock(&queue->m_mutex);

    struct timeval now;
    struct timespec time;
    gettimeofday(&now, NULL);
    time.tv_sec = now.tv_sec+ queue->m_timeout / 1000;
    time.tv_nsec = now.tv_usec*1000 + queue->m_timeout % 1000 *1000000 ;
    if(time.tv_nsec > 1000000000){  //用比较 加 减 代替乘除求余
        time.tv_sec += 1;
        time.tv_nsec -= 1000000000;
    }
    int ret = 0;
    while(cf_list_is_empty(queue->m_queue) == true && ret == 0){
        if(queue->m_timeout > 0)
            ret = pthread_cond_timedwait(&queue->m_condition,&queue->m_mutex,&time);
        else
            ret = pthread_cond_wait(&queue->m_condition,&queue->m_mutex);
    }
    if(ret == 0)
    {
        item = cf_list_take_front(queue->m_queue);
        if(cf_list_length(queue->m_queue) == queue->m_max_size - 1)
        {
            pthread_cond_signal(&queue->m_condition);
        }
    }
    else{//超时或错误
        cf_log(NULL,CF_LOG_DEBUG,"超时！！\n");
    }
    pthread_mutex_unlock(&queue->m_mutex);
    return item;
}

void* cf_async_queue_try_pop(struct cf_async_queue* queue)
{
    void* item = NULL;
    pthread_mutex_lock(&queue->m_mutex);
    if(cf_list_length(queue->m_queue) > 0)
        item = cf_list_take_front(queue->m_queue);
    pthread_mutex_unlock(&queue->m_mutex);
    return item;
}
bool cf_async_queue_try_push(struct cf_async_queue* queue,void* item){
    pthread_mutex_lock(&queue->m_mutex);
    int max_size = queue->m_max_size;
    if(max_size > 0 && cf_list_length(queue->m_queue) >= max_size ){
        pthread_mutex_unlock(&queue->m_mutex);
        return false;
    }
    cf_list_push(queue->m_queue,item);
    if(cf_list_length(queue->m_queue) == 1)
    {
        pthread_cond_signal(&queue->m_condition);
    }
    pthread_mutex_unlock(&queue->m_mutex);
    return true;
}
//在push阻塞时候(queue满)，queue释放，未做处理，待完善
void cf_async_queue_push(struct cf_async_queue* queue,void* item){
    pthread_mutex_lock(&queue->m_mutex);
    int max_size = queue->m_max_size;
    if(max_size > 0  ){
        while(cf_list_length(queue->m_queue) >= max_size){
            pthread_cond_wait(&queue->m_condition,&queue->m_mutex);
        }
    }
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

