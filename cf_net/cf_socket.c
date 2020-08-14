#include "cf_std.h"
#include "cf_socket.h"
#include "cf_allocator/cf_allocator_simple.h"
#include "cf_socket_linux.h"
#include "cf_socket_internal.h"

// typedef struct cf_socket {
//     cf_socket_priv* priv;
// }cf_socket;

cf_socket* cf_tcp_socket_create(uint16_t port){

    //cf_socket* sock = cf_allocator_simple_alloc(sizeof(cf_socket));
    //sock->priv = cf_tcp_socket_create_linux(port);
    //return sock;
    return cf_tcp_socket_create_linux(port);
    
}

int cf_socket_server_run(cf_socket* sock,void (*on_new_socket)(cf_socket* ,cf_socket*),
                void (*on_client_read)(cf_socket* ,uint8_t*,size_t len)){
    sock->on_new_socket = on_new_socket;
    sock->on_client_read = on_client_read;
    return sock->inf->run(sock);
}
cf_socket* cf_socket_pending_connection(cf_socket* sock){

}