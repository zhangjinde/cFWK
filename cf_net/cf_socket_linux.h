#ifndef CF_SOCKET_LINUX_H
#define CF_SOCKET_LINUX_H
#include <stdint.h>
#include "cf_socket.h"
cf_socket* cf_tcp_socket_create_linux(uint16_t port);

#endif//CF_SOCKET_LINUX_H