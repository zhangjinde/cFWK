#include "cf_allocator_simple.h"
#include <stdlib.h>
static size_t mem_alloc_size = 0;
void* cf_allocator_simple_alloc(size_t size){
    mem_alloc_size+=size;
    void* mem = calloc(1,size+sizeof(size_t));
    *((size_t*)mem) = size;
    return mem+sizeof(size_t);
}
void cf_allocator_simple_free(void* mem){
    mem_alloc_size -= *((size_t*)(mem-sizeof(size_t)));
    free(mem-sizeof(size_t));
}
void* cf_allocator_realloc(void* mem,size_t size)
{
    size_t old_size = *((size_t*)(mem-sizeof(size_t)));
    void* new_mem = realloc(mem-sizeof(size_t),size+sizeof(size_t));
    if(new_mem)
    {
        *((size_t*)new_mem) = size;
        mem_alloc_size += (size - old_size);
    }
    return new_mem+sizeof(size_t);
}
size_t cf_allocator_alloc_size()
{
    return mem_alloc_size;
}