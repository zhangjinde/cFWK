#include "cf_module.h"
#include "cf_allocator/cf_allocator_simple.h"
#include "cf_collection/cf_string.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
typedef struct cf_mod{
    cf_string* name;
    uint32_t version;
    void(*init)(void);
    void(*deinit)(void);
    bool is_init;
}cf_mod;
static cf_list* mod_list = NULL;
cf_mod* cf_mod_create(void){
    cf_mod* mod = cf_allocator_simple_alloc(sizeof(cf_mod));
    memset(mod,0,sizeof(cf_mod));
    mod->name = cf_string_create();
    return mod;
}
void cf_mod_setup(void){
    for(cf_iterator it = cf_mod_begin();!cf_iterator_is_end(&it);cf_iterator_next(&it)){
        cf_mod* mod = (cf_mod*)cf_iterator_get(&it);
        if(mod->is_init == false && mod->init){
            mod->init();
            mod->is_init = true;
        }
            
    }
}
void cf_mod_set_name(cf_mod* mod,const char* name){
    cf_string_set(mod->name,name);
}
void cf_mod_set_version(cf_mod* mod,uint32_t version){
    mod->version = version;
}
void cf_mod_set_init(cf_mod* mod,void(*init_f)(void)){
    mod->init = init_f;
}
void cf_mod_set_deinit(cf_mod* mod,void(*deinit_f)(void)){
    mod->deinit = deinit_f;
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