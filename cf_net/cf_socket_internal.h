#ifndef CF_SOCKET_INTERNAL_H
#define CF_SOCKET_INTERNAL_H

typedef struct cf_socket_inf {
    int (*run)(cf_socket* );
    cf_socket* (*pending_connection)(cf_socket* );
}cf_socket_inf;
typedef struct cf_socket {
    const cf_socket_inf* inf;
    void* instance;
    void (*on_new_socket)(cf_socket* ,cf_socket* );
    void (*on_client_read)(cf_socket* ,uint8_t*,size_t len );
}cf_socket;
#endif//CF_SOCKET_INTERNAL_H