#include <stdio.h>
#include "cf_test/cf_test.h"

#include "cf_module_test.h"
#include "cf_list_test.h"
#include "cf_factory_test.h"
#include "cf_websocket_test.h"

int main(){
    cf_suite* suite = cf_suite_create();
    cf_suite_add_suite(suite,get_module_test_suite());
    cf_suite_add_suite(suite,get_list_test_suite());
    cf_suite_add_suite(suite,get_factory_test_suite());
    cf_suite_add_suite(suite,get_websocket_test_suite());
    cf_suite_run(suite);
    printf("%s",cf_suite_msg(suite));
    cf_suite_destroy(suite);
    return 0;
}
