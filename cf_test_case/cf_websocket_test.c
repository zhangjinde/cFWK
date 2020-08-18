#include "cf_websocket_test.h"
#include "cf_websocket/cf_websocket_server.h"
#include <stddef.h>
#include <stdio.h>
static void on_new_websocket(cf_websocket_server* server,cf_websocket* new_cli){
    
}
static void on_cli_read(cf_websocket* new_cli,const uint8_t* buf,size_t n){
    printf("data=%s\n",buf);
}
void test_websocket_case(cf_test* tc){
    cf_websocket_server* server = cf_websocket_server_create(8800);
    cf_websocket_server_run(server,on_new_websocket,on_cli_read);
    return;
}
cf_suite* get_websocket_test_suite(){
    cf_suite* suite = cf_suite_create();
    cf_suite_add_test(suite,cf_test_create("websocket_test_case",test_websocket_case));
    return suite;
}
