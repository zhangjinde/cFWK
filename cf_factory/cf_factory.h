#include "cf_element.h"

typedef struct cf_factory cf_factory;
cf_factory* cf_factory_create(const char* factory_name);
void cf_factory_add_factory(cf_factory* factory);
cf_factory* cf_find_factory(const char* factory_name);
cf_element* cf_factory_create_elem(cf_factory* factory,const char* element_name);
const char* cf_factory_get_name(cf_factory* factory);
    