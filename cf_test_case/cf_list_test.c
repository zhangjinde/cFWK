#include "cf_list_test.h"
#include "cf_collection/cf_list.h"
#include <stdio.h>
static void list_test_case(cf_test* tc){
    long int_arr[] = {10,20,30};
    cf_list* list  = cf_list_create(NULL);
    CF_ASSERT(tc,NULL,cf_list_is_empty(list) == true);

    cf_list_push(list,(void*)int_arr[0]);
    cf_list_push(list,(void*)int_arr[1]);
    cf_list_push(list,(void*)int_arr[2]);
    CF_ASSERT(tc,NULL,cf_list_is_empty(list) == false);
    CF_ASSERT(tc,NULL,cf_list_length(list) == 3);
    int inx = 0;
    for(cf_iterator it = cf_list_begin(list);!cf_iterator_is_end(&it);cf_iterator_next(&it)){
        long d = (long)cf_iterator_get(&it);
        CF_ASSERT(tc,NULL,d == int_arr[inx++]);
    }
    CF_ASSERT(tc,NULL,cf_list_length(list) == 3);
    cf_list_clear(list);
    CF_ASSERT(tc,NULL,cf_list_length(list) == 0);
    CF_ASSERT(tc,NULL,cf_list_is_empty(list) == true);

    cf_list_push(list,(void*)int_arr[0]);
    cf_list_push(list,(void*)int_arr[1]);
    cf_list_push(list,(void*)int_arr[2]);
    
    CF_ASSERT(tc,NULL,cf_list_length(list) == 3);
    inx = 0;
    while(cf_list_length(list) > 0){
        long d = (long)cf_list_take_front(list);
        CF_ASSERT(tc,NULL,d == int_arr[inx++]);
    }
    CF_ASSERT(tc,NULL,cf_list_is_empty(list) == true);
    cf_list_destroy(list);
    
}

cf_suite* get_list_test_suite(){
    cf_suite* suite =  cf_suite_create();
    cf_suite_add_test(suite,cf_test_create("list_test_case",list_test_case));
    return suite;
}