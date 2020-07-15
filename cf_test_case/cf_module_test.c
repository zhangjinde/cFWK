#include <stddef.h>
#include <string.h>
#include "cf_test/cf_test.h"
#include "cf_module_test.h"
#include "cf_module/cf_module.h"

static void test_module_case(cf_test* tc){
    CF_ASSERT(tc,NULL,cf_mod_count() >= 1);
    cf_iterator it = cf_mod_begin();
    for(cf_iterator it = cf_mod_begin();!cf_iterator_is_end(&it);cf_iterator_next(&it)){
        cf_mod* mod = (cf_mod*)cf_iterator_get(&it);
        if(strcmp("test_mod",cf_mod_get_name(mod)) == 0)
            break;
    }
    CF_ASSERT(tc,NULL,!cf_iterator_is_end(&it));
}

cf_suite* get_module_test_suite(){
    cf_suite* suite = cf_suite_create();
    cf_suite_add_test(suite,cf_test_create("module_test_case",test_module_case));
    return suite;
}
static void test_mod_load_func(void){

}
static void test_mod_unload_func(void){

}
static void test_mod_init(cf_mod* mod){
    cf_mod_set_name(mod,"test_mod");
    cf_mod_set_version(mod,CF_MOD_MK_VERSION(0,0,0));
    cf_mod_set_load(mod,test_mod_load_func);
    cf_mod_set_unload(mod,test_mod_unload_func);
}

CF_MOD_INIT(test_mod_init)
