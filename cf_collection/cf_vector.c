#include "cf_vector.h"
#include "cf_allocator/cf_allocator_simple.h"
#include "cf_iterator.h"
#include <string.h>
#include <stdint.h>
#define CF_VECTOR_DEFAULT_BUFF_SIZE     32
struct cf_vector{
    void* m_buffer;
    size_t m_elem_size;
    size_t m_elem_count;
    size_t m_capacity;
};

struct cf_vector* cf_vector_create(size_t elem_size,size_t count){
    struct cf_vector* vector = (struct cf_vector*)cf_allocator_simple_alloc(sizeof(struct cf_vector));
    size_t capacity = elem_size*count;
    if(capacity < CF_VECTOR_DEFAULT_BUFF_SIZE)
        capacity = CF_VECTOR_DEFAULT_BUFF_SIZE;
    vector->m_buffer = cf_allocator_simple_alloc(capacity);
    vector->m_elem_size = elem_size;
    vector->m_elem_count = count;
    vector->m_capacity = capacity;
    
    return vector;
}
void* cf_vector_buffer(struct cf_vector* vector){
    return vector->m_buffer;
}
size_t cf_vector_length(struct cf_vector* vector){
    return vector->m_elem_count;
}
int cf_vector_resize(struct cf_vector* vector,size_t count){
    int ret = 0;
    if(vector->m_elem_size*count > vector->m_capacity)
    {
        size_t new_capacity = vector->m_elem_size*count;
        new_capacity+=new_capacity / 2;
        void* new_mem = cf_allocator_simple_realloc (vector->m_buffer,new_capacity );
        if(new_mem)
        {
            vector->m_buffer = new_mem;
            vector->m_capacity = new_capacity;
            vector->m_elem_count = count;
        }
        else
        {
            ret = -1;
        }
    }else
    {
        vector->m_elem_count = count;
    }
    
    return ret;
}
int cf_vector_append(struct cf_vector* vector,void* array,size_t count)
{
    int ret = 0;
    ret = cf_vector_resize(vector,vector->m_elem_count+count);
    if(ret == 0)
    {
        memcpy((uint8_t*)vector->m_buffer+vector->m_elem_size*vector->m_elem_count,array,vector->m_elem_size*count);
    }
    return ret;
}
    
void cf_vector_delete(struct cf_vector* vector)
{
    if(vector->m_buffer)
        cf_allocator_simple_free(vector->m_buffer);
    cf_allocator_simple_free(vector);
}
static bool is_end(struct cf_iterator* iter){
    return (long)iter->m_priv == ((struct cf_vector*)iter->m_container)->m_elem_count;
}
static void next(struct cf_iterator* iter){
    iter->m_priv = (void*)(((long )iter->m_priv)+1);   
}
static void _remove(struct cf_iterator* iter){
    struct cf_vector* v = (struct cf_vector*)iter->m_container;
    memmove((uint8_t*)v->m_buffer+(long)iter->m_priv * v->m_elem_size,(uint8_t*)v->m_buffer+(long)((uint8_t*)iter->m_priv+1) * v->m_elem_size,v->m_elem_size*(v->m_elem_count-(long)iter->m_priv-1));
    iter->m_priv = (void*)(((size_t)iter->m_priv)-1);
}
static void* get(struct cf_iterator* iter){
    struct cf_vector* v = (struct cf_vector*)iter->m_container;
    return (uint8_t*)v->m_buffer+(long)iter->m_priv * v->m_elem_size;
}

static const struct cf_iterator_vt cf_vector_iterator_vt = 
{
    .is_end = is_end,
    .next = next,
    .get = get,
    .remove = _remove
    
};



struct cf_iterator cf_vector_begin(struct cf_vector* v)
{
    struct cf_iterator iter;
    iter.m_priv = 0;
    iter.m_vt = &cf_vector_iterator_vt;
    iter.m_container = v;
    return iter;
}