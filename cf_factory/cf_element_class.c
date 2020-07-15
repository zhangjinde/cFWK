#include <stddef.h>
#include "cf_element_class.h"
#include "cf_collection/cf_string.h"
#include "cf_allocator/cf_allocator_simple.h"
#include "cf_element_priv.h"
#include "cf_std.h"

typedef struct cf_element_class{
    cf_string* name;
    cf_element_constructor constructor;
    cf_element_deconstructor deconstructor;
    cf_element_start_method start_method;
} cf_element_class;

cf_element_class* cf_element_class_create(const char* class_name){
    cf_element_class* e_class = cf_allocator_simple_alloc(sizeof(cf_element_class));
    e_class->name = cf_string_create_from_cstr(class_name);
}
void cf_element_class_set_constructor(cf_element_class* elem_class,cf_element_constructor constructor){
    elem_class->constructor = constructor;
}
void cf_element_class_set_deconstructor(cf_element_class* elem_class,cf_element_deconstructor deconstructor){
    elem_class->deconstructor = deconstructor;
}

cf_element_constructor cf_element_class_get_constructor(cf_element_class* elem_class){
    return elem_class->constructor;
}
void cf_element_class_set_start_method(cf_element_class* elem_class,cf_element_start_method start){
    elem_class->start_method = start;
}
cf_element_start_method cf_element_class_get_start_method(cf_element_class* elem_class)
{
    return elem_class->start_method;
}
const char* cf_element_class_get_name(cf_element_class* elem_class){
    if(elem_class == NULL) return NULL;
    return cf_string_c_str(elem_class->name);
}
cf_element* cf_element_class_create_element(cf_element_class* elem_class,const char* elem_name){
    return cf_element_create(elem_name,elem_class);
}