
#include "cf_test/cf_test.h"
#include "cf_util/cf_util.h"
#include "cf_std.h"
#include <stdio.h>
#include <string.h>
static void test_base64_case(cf_test* tc){
    uint8_t res[128];
    cf_base64_encode("a123456",res,sizeof(res));
    //printf("a123456->(base64)->%s\n",res);
    CF_ASSERT(tc,NULL,strcmp(res,"YTEyMzQ1Ng==") == 0);
    cf_base64_decode("YTEyMzQ1Ng==",res,sizeof(res));
    //printf("%s<-(base64)<-YTEyMzQ1Ng==\n",res);
    CF_ASSERT(tc,NULL,strcmp(res,"a123456") == 0);
    return;
}

cf_suite* get_base64_test_suite(){
    cf_suite* suite = cf_suite_create();
    cf_suite_add_test(suite,cf_test_create("base64_test_case",test_base64_case));
    return suite;
}