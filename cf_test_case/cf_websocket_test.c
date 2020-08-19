#include "cf_websocket_test.h"
#include "cf_websocket/cf_websocket_server.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
static void on_new_websocket(cf_websocket_server* server,cf_websocket* new_cli){
    printf("connect new cli[%p]\n",new_cli);
}
static void on_disconnect(cf_websocket_server* server,cf_websocket* cli){
    printf("disconnect cli[%p]\n",cli);
}
static void on_cli_read(cf_websocket* cli,const char* buf,uint64_t n){
    printf("data=%s\n",buf);
    uint8_t buffer[256];
    sprintf(buffer,"hello %s\r\n",buf);
    cf_websocket_write_text(cli,buffer,strlen(buffer));
}
void test_websocket_case(cf_test* tc){
    cf_websocket_server* server = cf_websocket_server_create(8800);
    cf_websocket_server_set_on_connect_callback(server,on_new_websocket);
    cf_websocket_server_set_on_disconnect_callback(server,on_disconnect);
    cf_websocket_server_set_on_read_text_callback(server,on_cli_read);
    cf_websocket_server_run(server);

    return;
}
cf_suite* get_websocket_test_suite(){
    cf_suite* suite = cf_suite_create();
    cf_suite_add_test(suite,cf_test_create("websocket_test_case",test_websocket_case));
    return suite;
}
