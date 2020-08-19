#ifndef CF_SOCKET_H
#define CF_SOCKET_H
#include <stdint.h>
#include <stddef.h>
typedef struct cf_socket cf_socket;
cf_socket* cf_tcp_socket_create(uint16_t port);
int cf_socket_server_run(cf_socket* sock,void (*on_new_socket)(cf_socket* sock,cf_socket*),
        void (*on_client_read)(cf_socket* ,uint8_t*,size_t len));
int cf_socket_write(cf_socket* sock,const uint8_t* buf,size_t n);
void cf_socket_set_user_data(cf_socket* sock,void* user_data);
void* cf_socket_get_user_data(cf_socket* sock);
#endif//#define CF_SOCKET_H