#include "cf_std.h"
#include "cf_allocator/cf_allocator_simple.h"
#include "cf_net/cf_socket.h"
#include "cf_websocket_server.h"
#include "cf_http/cf_http_parser.h"
#include "string.h"
#include <stdbool.h>
#include <stdio.h>
#define CF_WEBSOCKET_RECV_BUFF_LEN  1024
typedef struct cf_websocket_server {
    cf_socket* sock;
    void (*on_new_websocket)(cf_websocket_server* ,cf_websocket* );
}cf_websocket_server;
typedef enum cf_sock_state{
    INIT = 0,
    CONNECTED
}cf_sock_state;
typedef struct cf_websocket {
    cf_sock_state state;
    cf_socket* sock;
    uint8_t recv_buffer[CF_WEBSOCKET_RECV_BUFF_LEN];
    uint32_t recv_len;
}cf_websocket;
cf_websocket_server* cf_websocket_server_create(uint16_t port){
    cf_websocket_server* server = cf_allocator_simple_alloc(sizeof(cf_websocket_server));
    server->sock = cf_tcp_socket_create(port);
    cf_socket_set_user_data(server->sock,server);
    return server;
}

static void on_new_socket(cf_socket* server,cf_socket* new_cli){
    cf_websocket* websock = cf_allocator_simple_alloc(sizeof(cf_websocket));
    websock->sock = new_cli;
    cf_websocket_server* ws_server = cf_socket_get_user_data(server);
    // if(ws_server->on_new_websocket)
    //     ws_server->on_new_websocket(ws_server,websock);
    cf_socket_set_user_data(new_cli,websock);
    return;
}
static void on_client_read(cf_socket* client,uint8_t* buffer,size_t len){
    cf_websocket* ws_sock = cf_socket_get_user_data(client);
    if(ws_sock->state == INIT){
        if(ws_sock->recv_len == 0){
            size_t parsed_len = 0;
            cf_http_request* request = cf_http_parse(buffer,len,&parsed_len);
            if(request != NULL && strcmp(cf_http_request_method(request),"GET") == 0 && 
                            strcmp(cf_http_request_upgrade(request),"websocket") == 0 ){
                                char accept_key[128];
                                memset(accept_key,0,sizeof(accept_key));
                                const char* key = cf_http_request_upgrade();
                                memcpy(accept_key,key,strlen(key));
                                strncat(accept_key,key,strlen(accept_key)+1);

                
            }


        }
        
        printf("size=%d\n",len);
        printf("data - %s\n",(char*)buffer);
    }

}

int cf_websocket_server_run(cf_websocket_server* server,void (*on_new_websocket)(cf_websocket_server* ,cf_websocket* ),
    void (*on_cli_read)(cf_websocket* )){
    server->on_new_websocket = on_new_websocket;
    cf_socket_server_run(server->sock,on_new_socket,on_client_read);
    return CF_OK;
}