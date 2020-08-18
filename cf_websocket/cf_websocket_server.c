#include "cf_std.h"
#include "cf_allocator/cf_allocator_simple.h"
#include "cf_net/cf_socket.h"
#include "cf_websocket_server.h"
#include "cf_http/cf_http_parser.h"
#include "cf_util/cf_util.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#define CF_WEBSOCKET_RECV_BUFF_LEN  1024
typedef struct cf_websocket_server {
    cf_socket* sock;
    void (*on_new_websocket)(cf_websocket_server* ,cf_websocket* );
    void (*on_read)(cf_websocket* ,const uint8_t* buf,size_t n);
}cf_websocket_server;
typedef enum cf_sock_state{
    INIT = 0,
    CONNECTED
}cf_sock_state;
typedef struct cf_websocket {
    cf_sock_state state;
    cf_websocket_server* server;
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
    cf_websocket_server* ws_server = cf_socket_get_user_data(server);
    cf_websocket* websock = cf_allocator_simple_alloc(sizeof(cf_websocket));
    websock->sock = new_cli;
    websock->server = ws_server;
    
    if(ws_server->on_new_websocket)
        ws_server->on_new_websocket(ws_server,websock);
    cf_socket_set_user_data(new_cli,websock);
    return;
}
char *wb_accept = "HTTP/1.1 101 Switching Protocols\r\n" \
				  "Upgrade:websocket\r\n" \
				  "Connection: Upgrade\r\n" \
				  "Sec-WebSocket-Accept: %s\r\n" \
				  "WebSocket-Location: ws://%s%s\r\n" \
				  "WebSocket-Protocol:chat\r\n\r\n";

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
                                const char* key = cf_http_request_ws_key(request);
                                memcpy(accept_key,key,strlen(key));
                                strncat(accept_key,"258EAFA5-E914-47DA-95CA-C5AB0DC85B11",strlen("258EAFA5-E914-47DA-95CA-C5AB0DC85B11")+1);
                                uint8_t sha1[20];
                                cf_sha1_generate(accept_key,sha1,strlen(accept_key));
                                uint8_t sha1_base64[64];
                                cf_base64_encode(sha1,sha1_base64,sizeof(sha1_base64));
                                char accept_buffer[256];
                                sprintf(accept_buffer,wb_accept,sha1_base64,cf_http_request_host(request),cf_http_request_resource(request));
                                cf_socket_write(client,accept_buffer,strlen(accept_buffer));
                                ws_sock->state = CONNECTED;
            }
        }
    }
    else{
        uint64_t data_len = buffer[1] & 0x7f;
        if(data_len == 126){
            data_len = buffer[3];
            data_len += buffer[2]<<8;
            buffer += 4;
        }
        else if(data_len == 127){
            data_len = buffer[9];
            data_len += buffer[8]<<8;
            data_len += buffer[7]<<16;
            data_len += buffer[6]<<24;
            data_len += (uint64_t)buffer[5]<<32;
            data_len += (uint64_t)buffer[4]<<40;
            data_len += (uint64_t)buffer[3]<<48;
            data_len += (uint64_t)buffer[2]<<56;
            buffer += 10;
        }
        else{
            buffer += 2;
        }
        uint8_t mask_key[4];
        memcpy(mask_key,buffer,4);
        buffer += 4;
        uint8_t data[2048];
        for(int i =0;i < data_len;i++){
            data[i] = buffer[i] ^ mask_key[i%4];
        }
        data[data_len] = '\0';
        if(ws_sock->server->on_read)
            ws_sock->server->on_read(ws_sock,data,data_len);

    }
    

}

int cf_websocket_server_run(cf_websocket_server* server,void (*on_new_websocket)(cf_websocket_server* ,cf_websocket* ),
    void (*on_cli_read)(cf_websocket* ,const uint8_t*,size_t)){
    server->on_new_websocket = on_new_websocket;
    server->on_read = on_cli_read;
    cf_socket_server_run(server->sock,on_new_socket,on_client_read);
    return CF_OK;
}