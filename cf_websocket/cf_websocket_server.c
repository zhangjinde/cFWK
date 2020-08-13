#include "cf_std.h"
#include "cf_allocator/cf_allocator_simple.h"
#include "cf_net/cf_socket.h"
#include "cf_websocket_server.h"
#include <stdbool.h>
typedef struct cf_websocket_server {
    cf_socket* sock;
}cf_websocket_server;
cf_websocket_server* cf_websocket_server_create(uint16_t port){
    cf_websocket_server* server = cf_allocator_simple_alloc(sizeof(cf_websocket_server));
    server->sock = cf_tcp_socket_create(port);
    return server;
}
int cf_websocket_server_run(cf_websocket_server* server){
    cf_socket_server_run();
    return CF_OK;
}