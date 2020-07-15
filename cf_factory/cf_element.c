#include <stddef.h>
#include "cf_element.h"
#include "cf_element_class.h"
#include "cf_collection/cf_string.h"
#include "cf_allocator/cf_allocator_simple.h"

typedef struct cf_element{
    cf_element_class* elem_class;
    cf_string* elem_name;
}cf_element;
void cf_element_start(cf_element* elem){

}

const char* cf_element_get_name(cf_element* elem){
    if(elem == NULL ) return NULL;
    return cf_string_c_str(elem->elem_name);
}

const char* cf_element_get_classname(cf_element* elem){
    if(elem == NULL ) return NULL;
    return cf_element_class_get_name(elem->elem_class);
}

cf_element* cf_element_create(const char* elem_name,cf_element_class* e_class){
    cf_element* e = cf_allocator_simple_alloc(sizeof(cf_element));
    e->elem_name = cf_string_create_from_c_str(elem_name);
    e->elem_class = e_class;
    return e;
}