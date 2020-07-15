#include "cf_test/cf_test.h"
#include "cf_module/cf_module.h"
#include "cf_factory/cf_factory.h"
#include <stdio.h>
void test_factory_case(cf_test* tc){
    cf_mod_setup();
    cf_factory* factory = cf_find_factory("test_factory");
    CF_ASSERT(tc,NULL,factory != NULL);
    CF_ASSERT_STR_EQUALS(tc,NULL,"test_factory",cf_factory_get_name(factory));
    cf_element* elem = cf_factory_create_elem(factory,"test_elem","elem1");
    CF_ASSERT(tc,NULL,elem != NULL);
    CF_ASSERT_STR_EQUALS(tc,NULL,"test_elem",cf_element_get_classname(elem));
    CF_ASSERT_STR_EQUALS(tc,NULL,"elem1",cf_element_get_name(elem));
    return;
}
cf_suite* get_factory_test_suite(){
    cf_suite* suite = cf_suite_create();
    cf_suite_add_test(suite,cf_test_create("module_test_case",test_factory_case));
    return suite;
}

static void test_mod_load_func(void){
    cf_factory* factory = cf_factory_create("test_factory");
    cf_element_class* elem_class = cf_element_create_class("test_elem");
    cf_factory_rigster_element(factory,elem_class);
    cf_factory_add_factory(factory);
}
static void test_mod_unload_func(void){

}
static void test_mod_init(cf_mod* mod){
    cf_mod_set_name(mod,"test_factory_mod");
    cf_mod_set_version(mod,CF_MOD_MK_VERSION(0,0,0));
    cf_mod_set_load(mod,test_mod_load_func);
    cf_mod_set_unload(mod,test_mod_unload_func);
}

CF_MOD_INIT(test_mod_init)