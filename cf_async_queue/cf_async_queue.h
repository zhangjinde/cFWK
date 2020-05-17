#ifndef CF_ASYNC_QUEUE_H
#define CF_ASYNC_QUEUE_H


#include "cf_collection/cf_list.h"
#include <stdint.h>
#ifdef __cplusplus
extern "C"{
#endif
typedef struct cf_async_queue cf_async_queue;
struct cf_async_queue* cf_async_queue_create(void (*free_data)(void*));
void cf_async_queue_set_max_size(struct cf_async_queue* queue,int max_size);    //默认max_size=0，如果设置了max_size，则当队列长度到达max_size时push将被阻塞
void cf_async_queue_set_timeout(struct cf_async_queue* queue,uint32_t timeout); //毫秒
void* cf_async_queue_pop(struct cf_async_queue*);
int cf_async_queue_length(struct cf_async_queue*);
void* cf_async_queue_try_pop(struct cf_async_queue*);
void cf_async_queue_push(struct cf_async_queue*,void* item);
void cf_async_queue_delete(struct cf_async_queue*);

#ifdef __cplusplus
}

#endif
#endif//CF_ASYNC_QUEUE_H
