#include <stddef.h>
#include "cf_test/cf_test.h"
#include "cf_module_test.h"
#include "cf_module/cf_module.h"

static void test_module_case(cf_test* tc){
    CF_ASSERT(tc,NULL,cf_mod_count() == 1);
    cf_iterator it = cf_mod_begin();
    cf_mod* mod = (cf_mod*)cf_iterator_get(&it);
    CF_ASSERT_STR_EQUALS(tc,NULL,"test_mod",cf_mod_get_name(mod));
}

cf_suite* get_module_test_suite(){
    cf_suite* suite = cf_suite_create();
    cf_suite_add_test(suite,cf_test_create("module_test_case",test_module_case));
    return suite;
}
static void test_mod_init_func(void){

}
static void test_mod_init_func(void){

}
static void test_mod_init(cf_mod* mod){
    cf_mod_set_name(mod,"test_mod");
    cf_mod_set_version(mod,CF_MOD_MK_VERSION(0,0,0));
    cf_mod_set_
}

CF_MOD_INIT(test_mod_init)
