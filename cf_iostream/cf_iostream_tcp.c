#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "cf_iostream_tcp.h"
#include "cf_allocator/cf_allocator_simple.h"
//毫秒
#define CF_IOSTREAM_TCP_CONNECT_TIMEOUT 2000
typedef struct cf_iostream_tcp {
    cf_iostream m_stream;
    int m_sock;
}cf_iostream_tcp;


#define IOSTREAM_TO_TCP_STREAM(stream)    (cf_iostream_tcp*)((uint8_t*)(stream) - (uint8_t*)&((cf_iostream_tcp*)0)->m_stream)


static void scp_stream_close(cf_iostream* stream){
    cf_iostream_tcp* tcp_stream = IOSTREAM_TO_TCP_STREAM(stream);
    if(tcp_stream->m_sock >= 0)
        close(tcp_stream->m_sock);
    tcp_stream->m_sock = -1;
}
static void scp_stream_destroy(cf_iostream* stream){
    cf_iostream_tcp* tcp_stream = IOSTREAM_TO_TCP_STREAM(stream);
    scp_stream_close(stream);
    tcp_stream->m_sock = -1;
    cf_allocator_simple_free(tcp_stream);
}
static int scp_stream_writeln(cf_iostream* stream,const char* str){

    cf_iostream_tcp* tcp_stream = IOSTREAM_TO_TCP_STREAM(stream);
    uint32_t size = strlen(str)+1;
    int ret = send(tcp_stream->m_sock, &size, sizeof(size), 0);
    if(ret <= 0)
        return -1;
    ret = 0;
    int send_cnt = 0;
    while(send_cnt < size){
        ret = send(tcp_stream->m_sock, str, size, 0);
        if(ret <= 0)
            return -1;
        send_cnt += ret;
    }
    
    return send_cnt;
}
static cf_iostream_vt tcp_stream_vt = {
    .writeln = scp_stream_writeln,
    .close = scp_stream_close,
    .destroy = scp_stream_destroy
};

cf_iostream* cf_iostream_tcp_connect(const char* ipaddr,uint16_t port){
    int sock = socket(PF_INET,SOCK_STREAM , 0);
    if(sock < 0){
        return NULL;
    }
    //cf_iostream_tcp* tcp_stream = cf_allocator_simple_alloc(sizeof(cf_iostream_tcp));
    //assert(tcp_stream->m_sock >= 0);
    struct sockaddr_in address;
    bzero(&address,sizeof(address));

    //转换成网络地址
    address.sin_port = htons(port);
    address.sin_family = AF_INET;
    //地址转换
    address.sin_addr.s_addr = inet_addr(ipaddr);
    fcntl(sock,F_SETFL,fcntl(sock,F_GETFL,0)|O_NONBLOCK);
    
    int ret = connect(sock, (struct sockaddr*) &address, sizeof(address));
    if(ret != 0){
        fd_set wset;
        FD_ZERO(&wset);
        FD_SET(sock, &wset);
        struct timeval timeout;
        timeout.tv_sec = CF_IOSTREAM_TCP_CONNECT_TIMEOUT/1000;
        timeout.tv_usec = (CF_IOSTREAM_TCP_CONNECT_TIMEOUT%1000) * 1000;
        ret = select(sock + 1, NULL, &wset, NULL, &timeout);
        if(ret != 1 || !FD_ISSET(sock,&wset)){  //超时
            close(sock);
            return NULL;
        }
    }
    cf_iostream_tcp* tcp_stream = (cf_iostream_tcp*)cf_allocator_simple_alloc(sizeof(cf_iostream_tcp));
    if(tcp_stream == NULL)
    {
        close(sock);
        return NULL;
    }
    tcp_stream->m_sock = sock;
    
    tcp_stream->m_stream.m_vt = &tcp_stream_vt;
    return &tcp_stream->m_stream;
}