#include "cf_std.h"
#include "cf_socket.h"
#include "cf_allocator/cf_allocator_simple.h"
typedef struct cf_socket {
}cf_socket;
cf_socket* cf_tcp_socket_create(uint16_t port){
    cf_socket* sock = cf_allocator_simple_alloc(sizeof(cf_socket));
    return sock;
}
int cf_socket_server_run(cf_socket* sock){
    return CF_OK;
}