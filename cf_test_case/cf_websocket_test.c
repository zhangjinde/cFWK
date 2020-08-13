#include "cf_websocket_test.h"
#include "cf_websocket/cf_websocket_server.h"
void test_websocket_case(cf_test* tc){
    cf_websocket_server* server = cf_websocket_server_create(8953);
    cf_websocket_server_run(server);
    return;
}
cf_suite* get_websocket_test_suite(){
    cf_suite* suite = cf_suite_create();
    cf_suite_add_test(suite,cf_test_create("websocket_test_case",test_websocket_case));
    return suite;
}
