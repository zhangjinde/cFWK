#ifndef CF_SOCKET_H
#define CF_SOCKET_H
#include <stdint.h>
#include <stddef.h>
typedef struct cf_socket cf_socket;
cf_socket* cf_tcp_socket_create(uint16_t port);
int cf_socket_server_run(cf_socket* sock,void (*on_new_socket)(cf_socket* sock,cf_socket*),
        void (*on_client_read)(cf_socket* ,uint8_t*,size_t len));
#endif//#define CF_SOCKET_H