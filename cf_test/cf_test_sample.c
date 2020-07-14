#include "cf_test.h"
#include <stdio.h>
static int sum(int a,int b){
    return a+b;
}
static void test_sum(cf_test* tc){
    int s = sum(5,8);
    CF_ASSERT(tc,"sum(5,8) == 13",s == 113 );
}

static void test_str_equls(cf_test* tc){
    CF_ASSERT_STR_EQUALS(tc,"hellow == hellow","hellow","hellow" );
}

static void test_group1_test1(cf_test* tc){
    CF_ASSERT_STR_EQUALS(tc,NULL,"test_group1_test1","test_group1_test1" );
}
static void test_group1_test2(cf_test* tc){
    CF_ASSERT_STR_EQUALS(tc,NULL,"test_group1_test2","test_group1_test2" );
}
static void test_group1_test3(cf_test* tc){
    CF_ASSERT_STR_EQUALS(tc,NULL,"test_group1_test3","test_group1_test3" );
}

static void test_group2_test1(cf_test* tc){
    CF_ASSERT_STR_EQUALS(tc,NULL,"test_group2_test1","test_group2_test1" );
}
static void test_group2_test2(cf_test* tc){
    CF_ASSERT_STR_EQUALS(tc,NULL,"test_group2_test2","test_group2_test2" );
}
static void test_group2_test3(cf_test* tc){
    CF_ASSERT_STR_EQUALS(tc,NULL,"test_group2_test3","test_group2_test3" );
}

static void test_group3_test1(cf_test* tc){
    CF_ASSERT_STR_EQUALS(tc,NULL,"test_group3_test1","test_group3_test1" );
}
static void test_group3_test2(cf_test* tc){
    CF_ASSERT_STR_EQUALS(tc,NULL,"test_group3_test2","test_group3_test2" );
}
static void test_group3_test3(cf_test* tc){
    CF_ASSERT_STR_EQUALS(tc,NULL,"test_group3_test3","test_group3_test3" );
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

    cf_suite* suite1 = cf_suite_create();
    cf_suite* suite2 = cf_suite_create();
    cf_suite* suite3 = cf_suite_create();

    cf_suite_add_test(suite1,cf_test_create("test_group1_test1",test_group1_test1));
    cf_suite_add_test(suite1,cf_test_create("test_group1_test2",test_group1_test2));
    cf_suite_add_test(suite1,cf_test_create("test_group1_test3",test_group1_test3));

    cf_suite_add_test(suite2,cf_test_create("test_group2_test1",test_group2_test1));
    cf_suite_add_test(suite2,cf_test_create("test_group2_test2",test_group2_test2));
    cf_suite_add_test(suite2,cf_test_create("test_group2_test3",test_group2_test3));

    cf_suite_add_test(suite3,cf_test_create("test_group3_test1",test_group3_test1));
    cf_suite_add_test(suite3,cf_test_create("test_group3_test2",test_group3_test2));
    cf_suite_add_test(suite3,cf_test_create("test_group3_test3",test_group3_test3));

    cf_suite_add_suite(suite,suite1);
    cf_suite_add_suite(suite,suite2);
    cf_suite_add_suite(suite,suite3);

    cf_suite_run(suite);
    printf("suite: %s\n",cf_suite_msg(suite));
    cf_suite_destroy(suite);
    return 0;
}