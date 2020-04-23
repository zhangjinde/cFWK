#ifndef CF_ASYNC_QUEUE_H
#define CF_ASYNC_QUEUE_H


#include "cf_collection/cf_list.h"

#ifdef __cplusplus
extern "C"{
#endif
struct cf_async_queue;
struct cf_async_queue* cf_async_queue_create(void (*free_data)(void*));
void* cf_async_queue_pop(struct cf_async_queue*);
void cf_async_queue_push(struct cf_async_queue*,void* item);
void cf_async_queue_delete(struct cf_async_queue*);

#ifdef __cplusplus
}

#endif
#endif//CF_ASYNC_QUEUE_H