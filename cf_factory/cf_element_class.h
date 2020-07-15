#include "cf_element.h"
typedef void(*cf_element_constructor)(cf_element*);
typedef void(*cf_element_deconstructor)(cf_element*);
typedef void(*cf_element_start_method)(cf_element*);
typedef struct cf_element_class cf_element_class;
cf_element_class* cf_element_class_create(const char* class_name);
const char* cf_element_class_get_name(cf_element_class* elem);
void cf_element_class_set_constructor(cf_element_class* elem_class,cf_element_constructor constructor);
void cf_element_class_set_deconstructor(cf_element_class* elem_class,cf_element_deconstructor deconstructor);
cf_element_constructor cf_element_class_get_constructor(cf_element_class* elem_class);
void cf_element_class_set_start_method(cf_element_class* elem_class,cf_element_start_method start);
cf_element_start_method cf_element_class_get_start_method(cf_element_class* elem_class);
cf_element* cf_element_class_create_element(cf_element_class* elem_class,const char* element_name);