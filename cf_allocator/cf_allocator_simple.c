#include "cf_allocator_simple.h"
#include <stdlib.h>
void* cf_allocator_simple_alloc(size_t size){
    return calloc(1,size);
}
void cf_allocator_simple_free(void* mem){
    free(mem);
}