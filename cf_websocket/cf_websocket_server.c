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

#define CF_WEBSOCKET_RECV_BUFF_LEN  (1024*64)
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

				  //
                  //
static const char *wb_accept = "HTTP/1.1 101 Switching Protocols\r\n" \
				  "Upgrade: websocket\r\n" \
				  "Connection: Upgrade\r\n" \
				  "Sec-WebSocket-Accept: %s\r\n" \
                  "WebSocket-Protocol:chat\r\n"  \
                  "WebSocket-Location: ws://%s%s\r\n" \
				  "Server: Echo Server\r\n" \
                  "Access-Control-Allow-Credentials: false\r\n" \
                  "Access-Control-Allow-Methods: GET\r\n" \
                  "Access-Control-Allow-Headers: content-type\r\n" \
                  "\r\n";

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
                                cf_sha1_generate((uint8_t*)accept_key,sha1,strlen(accept_key));
                                sha1[20] = '\0';
                                uint8_t sha1_base64[64];
                                cf_base64_encode(sha1,sha1_base64,sizeof(sha1_base64));
                                char accept_buffer[256];
                                sprintf(accept_buffer,wb_accept,sha1_base64,cf_http_request_host(request),cf_http_request_resource(request));
                                cf_socket_write(client,(uint8_t*)accept_buffer,strlen(accept_buffer));
                                ws_sock->state = CONNECTED;
            }
        }
    }
    else{
        memcpy(ws_sock->recv_buffer+ws_sock->recv_len,buffer,len);
        ws_sock->recv_len+=len;

        while(ws_sock->recv_len > 0){
            uint8_t* pBuf = ws_sock->recv_buffer;
        size_t buf_len = ws_sock->recv_len;
        cf_print_arr("ws recv: ",pBuf,buf_len);
        uint8_t code = pBuf[0] & 0x0f;
        uint64_t data_len = pBuf[1] & 0x7f;
        int protocol_head_len = 0;
        if(data_len == 126){
            data_len = pBuf[3];
            data_len += pBuf[2]<<8;
            pBuf += 4;
            protocol_head_len = 8;
        }
        else if(data_len == 127){
            data_len = pBuf[9];
            data_len += pBuf[8]<<8;
            data_len += pBuf[7]<<16;
            data_len += pBuf[6]<<24;
            data_len += (uint64_t)pBuf[5]<<32;
            data_len += (uint64_t)pBuf[4]<<40;
            data_len += (uint64_t)pBuf[3]<<48;
            data_len += (uint64_t)pBuf[2]<<56;
            pBuf += 10;
            protocol_head_len = 14;
        }
        else{
            pBuf += 2;
            protocol_head_len = 6;
        }
        if(  buf_len < data_len+protocol_head_len)
        {
            break;
        }
        uint8_t mask_key[4];
        memcpy(mask_key,pBuf,4);
        pBuf += 4;
        uint8_t data[1024*64];
        
        for(int i =0;i < data_len;i++){
            data[i] = pBuf[i] ^ mask_key[i%4];
        }
        data[data_len] = '\0';
        if(code == 1 && ws_sock->server->on_read_text)
            ws_sock->server->on_read_text(ws_sock,(char*)data,data_len);
        else if(code == 2 && ws_sock->server->on_read_binary)
            ws_sock->server->on_read_binary(ws_sock,data,data_len);
        else if(code == 8)//客户端请求断开连接
        {
            uint8_t close_buf[2] = {0x88,0x00};
            cf_socket_write(ws_sock->sock,close_buf,sizeof(close_buf));
        }
        pBuf = ws_sock->recv_buffer+data_len+protocol_head_len;
        ws_sock->recv_len -= data_len+protocol_head_len;
        if(ws_sock->recv_len > 0)
            memcpy(ws_sock->recv_buffer,pBuf,ws_sock->recv_len); // 如果处理完该包，websocket缓存还有数据，则数据前移， 就地拷贝可能有问题，待观察。
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
    uint8_t* buffer = cf_allocator_simple_alloc(n+32);
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
        buffer[7] = (n >> 16) & 0xff;
        buffer[8] = (n >> 8) & 0xff;
        buffer[9] = n & 0xff;
        payload = buffer + 10;
    }
    memcpy(payload,buf,n);
    int total_len = n+payload-buffer;
    int remain_len = total_len;
    while(remain_len > 0){
        int wt_len = cf_socket_write(ws->sock,buffer,remain_len);
        if(wt_len<0 || (remain_len>0 && wt_len == 0))
        {
            total_len = -1;
            break;
        }
        remain_len-= wt_len;
    }
    cf_allocator_simple_free(buffer);
    buffer = NULL;
    return total_len;
}
int cf_websocket_write_text(cf_websocket* ws,const char* buf,uint64_t n){
    return cf_websocket_write(ws,buf,n,1);
}
int cf_websocket_write_binary(cf_websocket* ws,const uint8_t* buf,uint64_t n){
    return cf_websocket_write(ws,(char*)buf,n,2);
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
