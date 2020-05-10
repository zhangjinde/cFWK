#include "cf_allocator_simple.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
static size_t mem_alloc_size = 0;
void* cf_allocator_simple_alloc(size_t size){
    mem_alloc_size+=size;
    void* mem = calloc(1,size+sizeof(size_t));
    *((size_t*)mem) = size;
    return (uint8_t*)mem+sizeof(size_t);
}
void* cf_allocator_simple_clone(void* src,size_t size){
    mem_alloc_size+=size;
    void* mem = malloc(size+sizeof(size_t));
    *((size_t*)mem) = size;
    uint8_t* new_buffer = (uint8_t*)mem+sizeof(size_t);
    memcpy(new_buffer,src,size);
    return new_buffer;

}
void cf_allocator_simple_free(void* mem){
    mem_alloc_size -= *((size_t*)((uint8_t*)mem-sizeof(size_t)));
    free((uint8_t*)mem-sizeof(size_t));
}
void* cf_allocator_simple_realloc(void* mem,size_t size)
{
    size_t old_size = *((size_t*)((uint8_t*)mem-sizeof(size_t)));
    void* new_mem = realloc((uint8_t*)mem-sizeof(size_t),size+sizeof(size_t));
    if(new_mem)
    {
        *((size_t*)new_mem) = size;
        mem_alloc_size += (size - old_size);
    }
    return (uint8_t*)new_mem+sizeof(size_t);
}
size_t cf_allocator_alloc_size()
{
    return mem_alloc_size;
}


#ifdef CF_ALLOCATOR_SIMPLE_TEST
/***********
g++ -g -DCF_ALLOCATOR_SIMPLE_TEST cf_allocator_simple.c -o cf_allocator_simple_test
 * **********/
int main(){
    void* mem = cf_allocator_simple_alloc(10);
    cf_allocator_simple_free(mem);
}
#endif