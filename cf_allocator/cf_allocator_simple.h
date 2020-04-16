#ifndef CF_ALLOCATOR_SIMPLE_H
#define CF_ALLOCATOR_SIMPLE_H
#include<stddef.h>
void* cf_allocator_simple_alloc(size_t size);
void cf_allocator_simple_free(void* mem);
#endif//CF_ALLOCATOR_SIMPLE_H