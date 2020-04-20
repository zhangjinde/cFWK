#include "cf_vector.h"
#include "cf_allocator/cf_allocator_simple.h"
#define CF_VECTOR_DEFAULT_BUFF_SIZE     32
struct cf_vector{
    void* m_buffer;
    size_t m_elem_size;
    size_t m_elem_count;
    size_t m_capacity;
};

struct cf_vector* cf_vector_create(size_t elem_size,size_t count){
    struct cf_vector* vector = cf_allocator_simple_alloc(sizeof(struct cf_vector));
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
int cf_vector_append(struct cf_vector* vector,void* array,size_t count)
{
    int ret = 0;
    if(vector->m_elem_size*(vector->m_elem_count+count) > vector->m_capacity)
    {
        size_t new_capacity = vector->m_elem_size*(vector->m_elem_count+count);
        new_capacity+new_capacity / 2;
        void* new_mem = cf_allocator_simple_realloc (vector->m_buffer,new_capacity );
        if(new_mem)
        {
            vector->m_buffer = new_mem;
            vector->m_capacity = new_capacity;
            
        }
        else
        {
            ret = -1;
        }
    }
    if(ret == 0)
    {
        memcpy(vector->m_buffer+vector->m_elem_size*vector->m_elem_count,array,vector->m_elem_size*count);
        vector->m_elem_count += count;
    }
    return ret;
}
void cf_vector_delete(struct cf_vector* vector);