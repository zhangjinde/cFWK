#include "cf_test.h"
#include <stdio.h>
int sum(int a,int b){
    return a+b;
}
void test_sum(cf_test* t){
    int s = sum(5,8);
    CF_ASSERT(t,"sum(5,8) == 13",s == 113 );
}
int main(){
    cf_test* test_case = cf_test_create("test_sum",test_sum);
    cf_test_run(test_case);
    printf("%s: %s\n",cf_test_name(test_case),cf_test_msg(test_case));
    cf_test_destroy(test_case);
    return 0;
}