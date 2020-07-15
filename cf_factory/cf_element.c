#include <stddef.h>
#include "cf_element.h"
#include "cf_element_class.h"
#include "cf_collection/cf_string.h"
#include "cf_allocator/cf_allocator_simple.h"

typedef struct cf_element{
    cf_element_class* elem_class;
    cf_string* elem_name;
    void* priv;
}cf_element;
void cf_element_start(cf_element* elem){
    cf_element_start_method start = cf_element_class_get_start_method(elem->elem_class);
    if(start != NULL){
        start(elem);
    }
}
void cf_element_set_priv(cf_element* elem,void* priv){
    elem->priv = priv;
}
void* cf_element_get_priv(cf_element* elem){
    return elem->priv;
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
    e->elem_name = cf_string_create_from_cstr(elem_name);
    e->elem_class = e_class;
    cf_element_constructor constructor = cf_element_class_get_constructor(e_class);
    if(constructor != NULL)
        constructor(e);
    return e;
}