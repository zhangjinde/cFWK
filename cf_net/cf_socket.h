#ifndef CF_SOCKET_H
#define CF_SOCKET_H
#include <stdint.h>
typedef struct cf_socket cf_socket;
cf_socket* cf_tcp_socket_create(uint16_t port);
int cf_socket_server_run(cf_socket* sock);
#endif//#define CF_SOCKET_H