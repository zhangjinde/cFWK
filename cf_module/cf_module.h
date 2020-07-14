#include "cf_collection/cf_list.h"

typedef struct cf_mod cf_mod;
void cf_mod_register(cf_mod* info);
int cf_mod_count();
cf_iterator cf_mod_begin();
void cf_mod_set_name(cf_mod* mod,const char* name);
void cf_mod_set_version(cf_mod* mod,uint32_t version);
void cf_mod_set_init(cf_mod* mod,void(*init_f)(void));
void cf_mod_set_deinit(cf_mod* mod,void(*deinit_f)(void));
const char* cf_mod_get_name();
cf_mod* cf_mod_create();
#define CF_MOD_MK_VERSION(major,minor,revision)    ((major << 16) | (minor << 8) | revision)
#define CF_MOD_INIT(mod_init_func)    \
    static void __attribute__((constructor)) _mod_ ## mod_info ## _init(){\
        cf_mod* mod = cf_mod_create();  \
        mod_init_func(mod); \
        cf_mod_register(mod); \
    } 
