#include "cf_std.h"
#include "cf_allocator/cf_allocator_simple.h"
#include "cf_net/cf_socket.h"
#include "cf_websocket_server.h"
#include <stdbool.h>
#include <stdio.h>
typedef struct cf_websocket_server {
    cf_socket* sock;
}cf_websocket_server;
cf_websocket_server* cf_websocket_server_create(uint16_t port){
    cf_websocket_server* server = cf_allocator_simple_alloc(sizeof(cf_websocket_server));
    server->sock = cf_tcp_socket_create(port);
    return server;
}

static void on_new_socket(cf_socket* server,cf_socket* new_cli){
}
static void on_client_read(cf_socket* client,uint8_t* buffer,size_t len){
    printf("size=%d\n",len);
    printf("data - %s\n",(char*)buffer);
}

int cf_websocket_server_run(cf_websocket_server* server,void (*on_new_websocket)(cf_websocket_server* ,cf_websocket* ),
    void (*on_cli_read)(cf_websocket* )){
    cf_socket_server_run(server->sock,on_new_socket,on_client_read);
    return CF_OK;
}