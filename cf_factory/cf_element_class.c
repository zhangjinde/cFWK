#include <stddef.h>
#include "cf_element_class.h"
#include "cf_collection/cf_string.h"
#include "cf_allocator/cf_allocator_simple.h"
#include "cf_element_priv.h"

typedef struct cf_element_class{
    cf_string* name;
} cf_element_class;

cf_element_class* cf_element_create_class(const char* class_name){
    cf_element_class* e_class = cf_allocator_simple_alloc(sizeof(cf_element_class));
    e_class->name = cf_string_create_from_c_str(class_name);
}

const char* cf_element_class_get_name(cf_element_class* elem_class){
    if(elem_class == NULL) return NULL;
    return cf_string_c_str(elem_class->name);
}
cf_element* cf_element_class_create_element(cf_element_class* elem_class,const char* elem_name){
    return cf_element_create(elem_name,elem_class);
}