#include "cf_test.h"
#include <stdio.h>
int sum(int a,int b){
    return a+b;
}
void test_sum(cf_test* tc){
    int s = sum(5,8);
    CF_ASSERT(tc,"sum(5,8) == 13",s == 113 );
}

void test_str_equls(cf_test* tc){
    CF_ASSERT_STR_EQUALS(tc,"hellow == hellow","hellow","hellow" );
}

int main(){
    cf_test* test_case = cf_test_create("test_sum",test_sum);
    cf_test_run(test_case);
    printf("%s: %s\n",cf_test_name(test_case),cf_test_msg(test_case));
    cf_test_destroy(test_case);
    printf("\n\n");

    cf_suite* suite = cf_suite_create();
    cf_suite_add_test(suite,cf_test_create("test_str_equls",test_str_equls));
    cf_suite_add_test(suite,cf_test_create("test_sum",test_sum));
    cf_suite_run(suite);
    printf("suite: %s\n",cf_suite_msg(suite));
    cf_suite_destroy(suite);
    return 0;
}