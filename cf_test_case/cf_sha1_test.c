#include "cf_sha1_test.h"
#include "cf_util/cf_util.h"
#include "string.h"
#include <stdio.h>
void test_sha1_case(cf_test* tc){
    char test_str[1024] = "this is a sha1 test.";
    uint8_t sha1[20];
    uint8_t sha1_ref[20] = {0x2a,0x67,0x69,0xaa,0x7d,0x71,0x2a,0x17,0x1f,0xb1,0x83,0xbf,0x33,0x04,0x9e,0xae,0x00,0xd1,0x82,0x9f};
    cf_sha1_generate((uint8_t*)test_str,sha1,strlen(test_str));

    CF_ASSERT(tc,NULL,memcmp(sha1,sha1_ref,sizeof(sha1)) == 0);
    return;
}
cf_suite* get_sha1_test_suite(){
    cf_suite* suite = cf_suite_create();
    cf_suite_add_test(suite,cf_test_create("sha1_test_case",test_sha1_case));
    return suite;
}