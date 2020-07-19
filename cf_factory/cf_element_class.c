#include <stddef.h>
#include <string.h>
#include "cf_element_class.h"
#include "cf_collection/cf_string.h"
#include "cf_collection/cf_hash.h"
#include "cf_allocator/cf_allocator_simple.h"
#include "cf_element_priv.h"
#include "cf_pad.h"
#include "cf_std.h"

typedef struct cf_element_class{
    cf_string* name;
    cf_hash* attr_map;
    cf_element_constructor constructor;
    cf_element_deconstructor deconstructor;
    cf_element_start_method start_method;
    cf_element_processer processer;
    cf_data_release_method data_release_method;
} cf_element_class;

cf_element_class* cf_element_class_create(const char* class_name){
    cf_element_class* e_class = cf_allocator_simple_alloc(sizeof(cf_element_class));
    e_class->name = cf_string_create_from_cstr(class_name);
    e_class->attr_map = cf_hash_create(cf_hash_str,cf_hash_str_equal,cf_allocator_simple_free,cf_allocator_simple_free);
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
void cf_element_class_register_attr(cf_element_class* elem_class,const char* attr_name,cf_element_attr_write_method attr_write,cf_element_attr_read_method attr_read){
    char* str = cf_allocator_simple_alloc(strlen(attr_name+1));
    strcpy(str,attr_name);
    void* attr_func_arr = cf_allocator_simple_alloc(2*sizeof(attr_write));
    attr_func_arr[0] = attr_write;
    attr_func_arr[1] = attr_read;
    cf_hash_insert(elem_class->attr_map,str,attr_func_arr);
}
cf_element_attr_read_method cf_element_class_get_attr_read_method(cf_element_class* elem_class,const char* attr_name){
    void* arr = cf_hash_get(elem_class->attr_map,attr_name,NULL);
    if(arr != NULL) 
        return arr[1];
    else
        return NULL;
}
cf_element_attr_write_method cf_element_class_get_attr_write_method(cf_element_class* elem_class,const char* attr_name){
    void* arr = cf_hash_get(elem_class->attr_map,attr_name,NULL);
    if(arr != NULL) 
        return arr[0];
    else
        return NULL;
}
void cf_element_class_set_processer(cf_element_class* elem_class,cf_element_processer processer){
    elem_class->processer = processer;
}
cf_element_processer cf_element_class_get_processer(cf_element_class* elem_class){
    return elem_class->processer;
}
void cf_element_class_set_data_release_method(cf_element_class* elem_class,cf_data_release_method release){
    elem_class->data_release_method = release;
}
cf_data_release_method cf_element_class_get_data_release_method(cf_element_class* elem_class){
    return elem_class->data_release_method;
}