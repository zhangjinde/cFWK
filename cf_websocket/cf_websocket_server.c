#include "cf_std.h"
#include "cf_allocator/cf_allocator_simple.h"
#include "cf_net/cf_socket.h"
#include "cf_websocket_server.h"
#include "cf_http/cf_http_parser.h"
#include "cf_util/cf_util.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "cf_util/cf_util.h"
#define CF_WEBSOCKET_RECV_BUFF_LEN  1024
typedef struct cf_websocket_server {
    cf_socket* sock;
    void* data;
    void (*on_new_websocket)(cf_websocket_server* ,cf_websocket* );
    void (*on_disconnect)(cf_websocket_server* ,cf_websocket* );
    void (*on_read_text)(cf_websocket* ,const char* buf,uint64_t n);
    void (*on_read_binary)(cf_websocket* ,const uint8_t* buf,uint64_t n);
}cf_websocket_server;
typedef enum cf_sock_state{
    INIT = 0,
    CONNECTED
}cf_sock_state;
typedef struct cf_websocket {
    cf_sock_state state;
    cf_websocket_server* server;
    void* data;
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
static void on_disconnect(cf_socket* server,cf_socket* cli){
    cf_websocket_server* ws_server = cf_socket_get_user_data(server);
    cf_websocket* websock = cf_socket_get_user_data(cli);
    
    if(ws_server->on_disconnect)
        ws_server->on_disconnect(ws_server,websock);
    cf_allocator_simple_free(websock);
    return;
}
static const char *wb_accept = "HTTP/1.1 101 Switching Protocols\r\n" \
				  "Upgrade:websocket\r\n" \
				  "Connection: Upgrade\r\n" \
				  "Sec-WebSocket-Accept: %s\r\n" \
				  "WebSocket-Location: ws://%s%s\r\n" \
				  "WebSocket-Protocol:chat\r\n\r\n";

static void on_client_read(cf_socket* client,const uint8_t* buffer,size_t len){
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
                                uint8_t sha1[21];
                                cf_sha1_generate(accept_key,sha1,strlen(accept_key));
                                sha1[20] = '\0';
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
        uint8_t code = buffer[0] & 0x0f;
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
        uint8_t data[1024*64];
        for(int i =0;i < data_len;i++){
            data[i] = buffer[i] ^ mask_key[i%4];
        }
        data[data_len] = '\0';
        if(code == 1 && ws_sock->server->on_read_text)
            ws_sock->server->on_read_text(ws_sock,data,data_len);
        else if(code == 2 && ws_sock->server->on_read_binary)
            ws_sock->server->on_read_binary(ws_sock,data,data_len);
        else if(code == 8)//客户端请求断开连接
        {
            uint8_t close_buf[2] = {0x88,0x00};
            cf_socket_write(ws_sock->sock,close_buf,sizeof(close_buf));
        }
    }
}

int cf_websocket_server_run(cf_websocket_server* server){
    cf_socket_set_on_connect_callback(server->sock,on_new_socket);
    cf_socket_set_on_disconnect_callback(server->sock,on_disconnect);
    cf_socket_set_on_read_callback(server->sock,on_client_read);
    cf_socket_server_run(server->sock);
    return CF_OK;
}
static int cf_websocket_write(cf_websocket* ws,const char* buf,uint64_t n,uint8_t code){
    uint8_t buffer[1024*1024];
    buffer[0] = 0x80 | code;
    uint8_t* payload = NULL;
    if(n < 126){
        buffer[1] = n;
        payload = buffer + 2;
    }
        
    else if(n <= 0xFFFF){
        buffer[1] = 126;
        buffer[2] = n >> 8;
        buffer[3] = n & 0xff;
        payload = buffer + 4;
    }
        
    else{
        buffer[1] = 127;
        buffer[2] = (n >> 56) & 0xff;
        buffer[3] = (n >> 48) & 0xff;
        buffer[4] = (n >> 40) & 0xff;
        buffer[5] = (n >> 32) & 0xff;
        buffer[6] = (n >> 24) & 0xff;
        buffer[7] = (n >> 16) & 0xff ;
        buffer[8] = (n >> 8) & 0xff;
        buffer[9] = n & 0xff;
        payload = buffer + 10;
    }
    memcpy(payload,buf,n);
    return cf_socket_write(ws->sock,buffer,n+payload-buffer); 
}
int cf_websocket_write_text(cf_websocket* ws,const char* buf,uint64_t n){
    return cf_websocket_write(ws,buf,n,1);
}
int cf_websocket_write_binary(cf_websocket* ws,const uint8_t* buf,uint64_t n){
    return cf_websocket_write(ws,buf,n,2);
}
void cf_websocket_server_set_on_read_text_callback(cf_websocket_server* server,void (*on_cli_read_text)(cf_websocket*,const char*,uint64_t )){
    server->on_read_text = on_cli_read_text;
}
void cf_websocket_server_set_on_read_binary_callback(cf_websocket_server* server,void (*on_cli_read_binary)(cf_websocket*,const uint8_t*,uint64_t )){
    server->on_read_binary = on_cli_read_binary;
}
void cf_websocket_server_set_on_connect_callback(cf_websocket_server* server,void (*on_new_websocket)(cf_websocket_server* ,cf_websocket* )){
    server->on_new_websocket = on_new_websocket;
}
void cf_websocket_server_set_on_disconnect_callback(cf_websocket_server* server,void (*on_disconnect)(cf_websocket_server* ,cf_websocket* )){
    server->on_disconnect = on_disconnect;
}


void cf_websocket_server_set_user_data(cf_websocket_server* server,void* d){
    server->data = d;
}
void* cf_websocket_server_get_user_data(cf_websocket_server* server){
    return server->data;
}

void cf_websocket_set_user_data(cf_websocket* ws,void* d){
    ws->data = d;
}
void* cf_websocket_get_user_data(cf_websocket* ws){
    return ws->data;
}