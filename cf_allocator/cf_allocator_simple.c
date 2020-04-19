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
size_t cf_allocator_alloc_size()
{
    return mem_alloc_size;
}