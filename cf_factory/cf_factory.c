#include <stddef.h>
#include "cf_allocator/cf_allocator_simple.h"
#include "cf_collection/cf_string.h"
#include "cf_collection/cf_list.h"
#include "cf_factory.h"
typedef struct cf_factory{
    cf_string* name;
}cf_factory;
cf_factory* cf_factory_create(const char* factory_name){
    cf_factory* f = cf_allocator_simple_alloc(sizeof(cf_factory));
    f->name = cf_string_create_frome_c_str(factory_name);
    return f;
}
static cf_list* factory_list = NULL;
void cf_factory_add_factory(cf_factory* factory){
    if(factory_list == NULL)
        factory_list = cf_list_create(NULL);
    cf_list_push(factory_list,factory);
}
cf_factory* cf_find_factory(const char* factory_name){
    if(factory_list == NULL) return NULL;
    for(cf_iterator it = cf_list_begin(factory_list);!cf_iterator_is_end(&it);cf_iterator_next(&it)){
        cf_factory* f = cf_iterator_get(&it);
        if(cf_string_equels_cstr(f->name,factory_name))
            return f;
    }
    return NULL;
}
cf_element* cf_factory_create_elem(cf_factory* factory,const char* element_name){
    return NULL;
}
const char* cf_factory_get_name(cf_factory* factory)
{
    if(factory == NULL) return NULL;
    return cf_string_c_str(factory->name);
}