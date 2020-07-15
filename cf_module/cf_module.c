#include "cf_module.h"
#include "cf_allocator/cf_allocator_simple.h"
#include "cf_collection/cf_string.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
typedef struct cf_mod{
    cf_string* name;
    cf_list* deps; // cf_list<cf_string>
    uint32_t version;
    void(*load)(void);
    void(*unload)(void);
    bool is_load;
}cf_mod;
static cf_list* mod_list = NULL;
cf_mod* cf_mod_create(void){
    cf_mod* mod = cf_allocator_simple_alloc(sizeof(cf_mod));
    memset(mod,0,sizeof(cf_mod));
    mod->name = cf_string_create();
    mod->deps = cf_list_create((void(*)(void*))cf_string_destroy);
    return mod;
}
static cf_mod* cf_find_mod(const char* mod_name){
    if(mod_list == NULL) return NULL;
    for(cf_iterator it = cf_list_begin(mod_list);!cf_iterator_is_end(&it);cf_iterator_next(&it)){
        cf_mod* mod = cf_iterator_get(&it);
        return mod;
    }
}
void cf_mod_setup(void){
    for(cf_iterator it = cf_mod_begin();!cf_iterator_is_end(&it);cf_iterator_next(&it)){
        cf_mod* mod = (cf_mod*)cf_iterator_get(&it);
        if(mod->is_load == false && mod->load){
            if(cf_list_length(mod->deps) > 0 ){
                for(cf_iterator it = cf_list_begin(mod->deps);!cf_iterator_is_end(&it);cf_iterator_next(&it)){
                    cf_string* dep_name = cf_iterator_get(&it);
                    cf_mod* dep_mod = cf_find_mod(cf_string_c_str(dep_name));
                    assert(dep_mod != NULL);
                    if(dep_mod == NULL){

                    }
                    if(dep_mod->is_load == false)
                    {
                        dep_mod->load();
                        dep_mod->is_load = true;
                    }
                }
            }
            mod->load();
            mod->is_load = true;
        }
            
    }
}
void cf_mod_set_name(cf_mod* mod,const char* name){
    cf_string_set(mod->name,name);
}
void cf_mod_add_dep(cf_mod* mod,const char* mod_name){
    for(cf_iterator it = cf_list_begin(mod->deps);!cf_iterator_is_end(&it);cf_iterator_next(&it)){
        cf_string* name = cf_iterator_get(&it);
        if(cf_string_equels_cstr(name,mod_name))
            return;
    }
    cf_list_push(mod->deps,cf_string_create_from_cstr(mod_name));
}
void cf_mod_set_version(cf_mod* mod,uint32_t version){
    mod->version = version;
}
void cf_mod_set_load(cf_mod* mod,void(*load)(void)){
    mod->load = load;
}
void cf_mod_set_unload(cf_mod* mod,void(*unload)(void)){
    mod->unload = unload;
}
const char* cf_mod_get_name(cf_mod* mod){
    return cf_string_c_str(mod->name);
}
void cf_mod_register(cf_mod* mod){
    if(mod_list == NULL)
        mod_list = cf_list_create(NULL);
    cf_list_push(mod_list,mod);
}
void cf_mod_clear(){
    cf_list_clear(mod_list);
}
int cf_mod_count(){
    return cf_list_length(mod_list);
}
cf_iterator cf_mod_begin(){
    return cf_list_begin(mod_list);
}