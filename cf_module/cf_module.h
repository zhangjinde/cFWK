#include "cf_collection/cf_list.h"
#include <stdint.h>
typedef struct cf_mod cf_mod;
void cf_mod_setup(void);
void cf_mod_register(cf_mod* info);
int cf_mod_count(void);
void cf_mod_clear(void);
cf_iterator cf_mod_begin();
void cf_mod_set_name(cf_mod* mod,const char* name);
void cf_mod_set_version(cf_mod* mod,uint32_t version);
void cf_mod_set_load(cf_mod* mod,void(*load)(void));
void cf_mod_set_unload(cf_mod* mod,void(*unload)(void));
const char* cf_mod_get_name(cf_mod* mod);
cf_mod* cf_mod_create(void);
#define CF_MOD_MK_VERSION(major,minor,revision)    ((major << 16) | (minor << 8) | revision)
#define CF_MOD_INIT(mod_init_func)    \
    static void __attribute__((constructor)) _mod_ ## mod_info ## _init(){\
        cf_mod* mod = cf_mod_create();  \
        mod_init_func(mod); \
        cf_mod_register(mod); \
    } 
