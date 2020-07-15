#include "cf_element.h"
typedef struct cf_element_class cf_element_class;
cf_element_class* cf_element_create_class(const char* class_name);
const char* cf_element_class_get_name(cf_element_class* elem);
cf_element* cf_element_class_create_element(cf_element_class* elem_class,const char* element_name);