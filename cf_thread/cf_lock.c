#include <pthread.h>
#include "cf_lock.h"
#include "cf_allocator/cf_allocator_simple.h"
typedef struct cf_lock {
    pthread_mutex_t mutex;
}cf_lock;

cf_lock* cf_lock_create(void){
    cf_lock* lock = cf_allocator_simple_alloc(sizeof(cf_lock));
    pthread_mutex_init(&lock->mutex,NULL);
}
void cf_lock_lock(cf_lock* lock){
    pthread_mutex_lock(&lock->mutex);
}
void cf_lock_unlock(cf_lock* lock){
    pthread_mutex_unlock(&lock->mutex);
}
void cf_lock_destroy(cf_lock* lock){
    pthread_mutex_destroy(&lock->mutex);
    cf_allocator_simple_free(lock);
}