#include "cf_element.h"
typedef int (*cf_element_constructor)(cf_element*);
typedef void(*cf_element_deconstructor)(cf_element*);
typedef void(*cf_element_start_method)(cf_element*);
typedef void* (*cf_element_processer)(cf_element*,void*);
typedef void* (*cf_element_attr_read_method)(cf_element*);
typedef void (*cf_element_attr_write_method)(cf_element*,void*);
typedef void (*cf_data_release_method)(void*);




typedef struct cf_element_class cf_element_class;
cf_element_class* cf_element_class_create(const char* class_name);
const char* cf_element_class_get_name(cf_element_class* elem);
void cf_element_class_set_constructor(cf_element_class* elem_class,cf_element_constructor constructor);
void cf_element_class_set_deconstructor(cf_element_class* elem_class,cf_element_deconstructor deconstructor);
cf_element_constructor cf_element_class_get_constructor(cf_element_class* elem_class);
void cf_element_class_set_start_method(cf_element_class* elem_class,cf_element_start_method start);
cf_element_start_method cf_element_class_get_start_method(cf_element_class* elem_class);

void cf_element_class_set_processer(cf_element_class* elem_class,cf_element_processer push);
cf_element_processer cf_element_class_get_processer(cf_element_class* elem_class);

void cf_element_class_set_data_release_method(cf_element_class* elem_class,cf_data_release_method release);
cf_data_release_method cf_element_class_get_data_release_method(cf_element_class* elem_class);

void cf_element_class_register_attr(cf_element_class* elem_class,const char* attr_name,cf_element_attr_write_method attr_write,cf_element_attr_read_method attr_read);
cf_element_attr_read_method cf_element_class_get_attr_read_method(cf_element_class* elem_class,const char* attr_name);
cf_element_attr_write_method cf_element_class_get_attr_write_method(cf_element_class* elem_class,const char* attr_name);




cf_element* cf_element_class_create_element(cf_element_class* elem_class,const char* element_name);