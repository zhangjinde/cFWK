#include <string.h>
#include "cf_string.h"
#include "cf_allocator/cf_allocator_simple.h"
#define CF_STRING_DEFAULT_CAPACITY  32

typedef struct cf_string {
    char* m_str;
    size_t m_capacity;
}cf_string;

cf_string* cf_string_create(void){
    cf_string* str = (cf_string*)cf_allocator_simple_alloc(sizeof(cf_string));
    if(str ){
        str->m_str = (char*)cf_allocator_simple_alloc(CF_STRING_DEFAULT_CAPACITY);
        str->m_capacity = CF_STRING_DEFAULT_CAPACITY;
    }
    return str;
}
cf_string* cf_string_create_frome_c_str(const char* c_str){
    size_t size = strlen(c_str);
    if(size+1 < CF_STRING_DEFAULT_CAPACITY)
        size = CF_STRING_DEFAULT_CAPACITY;
    cf_string* str = (cf_string*)cf_allocator_simple_alloc(sizeof(cf_string));
    if(str ){
        str->m_str = (char*)cf_allocator_simple_alloc(size);
        str->m_capacity = size;
        strcpy(str->m_str,c_str);
    }
    return str;
}
void cf_string_set(cf_string* str,const char* c_str){
    if(strlen(c_str)+1 < str->m_capacity){
        str->m_capacity = strlen(c_str)+CF_STRING_DEFAULT_CAPACITY;
        cf_allocator_simple_free(str->m_str);
        str->m_str = (char*)cf_allocator_simple_alloc(str->m_capacity);
    }
    strcpy(str->m_str,c_str);
}
const char* cf_string_c_str(cf_string* str){
    return str->m_str;
}
void cf_string_destroy(cf_string* str){
    cf_allocator_simple_free(str->m_str);
    cf_allocator_simple_free(str);
}