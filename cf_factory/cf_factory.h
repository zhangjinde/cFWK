#include "cf_element.h"
#include "cf_element_class.h"
typedef struct cf_factory cf_factory;
cf_factory* cf_factory_create(const char* factory_name);
void cf_factory_add_factory(cf_factory* factory);
void cf_factory_rigster_element(cf_factory* factory,cf_element_class* elem_class);
void cf_factory_unrigster_element_by_name(cf_factory* factory,const char* class_name);
cf_factory* cf_find_factory(const char* factory_name);
cf_element* cf_factory_create_elem(cf_factory* factory,const char* element_class_name,const char* element_name);
const char* cf_factory_get_name(cf_factory* factory);
    