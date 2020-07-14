#include "cf_module.h"
#include "cf_allocator/cf_allocator_simple.h"
#include "cf_collection/cf_string.h"

#include <stdio.h>
typedef struct cf_mod{
    cf_string* name;
}cf_mod;
static cf_list* mod_list = NULL;
cf_mod* cf_mod_create(void){
    cf_mod* mod = cf_allocator_simple_alloc(sizeof(cf_mod));
    mod->name = cf_string_create();
    return mod;
}
void cf_mod_set_name(cf_mod* mod,const char* name){
    cf_string_set(mod->name,name);
}
const char* cf_mod_get_name(cf_mod* mod){
    return cf_string_c_str(mod->name);
}
void cf_mod_register(cf_mod* mod){
    if(mod_list == NULL)
        mod_list = cf_list_create(NULL);
    cf_list_push(mod_list,mod);
}
int cf_mod_count(){
    return cf_list_length(mod_list);
}
cf_iterator cf_mod_begin(){
    return cf_list_begin(mod_list);
}