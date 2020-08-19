#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "cf_std.h"
#include "cf_socket_internal.h"
#include "cf_socket_linux.h"
#include "cf_allocator/cf_allocator_simple.h"
#include "cf_collection/cf_list.h"

#define SOCKET_BUFFER_SIZE (1024*128)

static int socket_run(cf_socket* cf_sock_server);
static int socket_write(cf_socket* sock,const uint8_t* buf,size_t n);
static const cf_socket_inf sock_inf = {
    .run = socket_run,
    .write = socket_write
};

static int socket_run(cf_socket* cf_sock_server){
    int sock = (int)(uintptr_t)cf_sock_server->instance;
    struct cf_list* cli_list = cf_list_create(NULL);
    int max_fd = sock;
    while(true){
        fd_set r_set,except_set;
        FD_ZERO(&r_set);
        FD_ZERO(&except_set);
        FD_SET(sock,&r_set);
        FD_SET(sock,&except_set);
        for(struct cf_iterator iter = cf_list_begin(cli_list);!cf_iterator_is_end(&iter);cf_iterator_next(&iter)){
            int sock_cli = (int)(uintptr_t)((cf_socket*)cf_iterator_get(&iter))->instance;
            FD_SET(sock_cli,&r_set);
            FD_SET(sock_cli,&except_set);
        }
        //memcpy(&except_set,&r_set,sizeof(r_set));
        int nready = select(max_fd+1,&r_set,NULL,&except_set,NULL);
        if(FD_ISSET(sock,&r_set))
        {
            nready--;
            cf_socket* cf_sock_cli = NULL;
            int sock_cli = accept(sock, NULL,NULL);
            if(cf_sock_server->on_new_socket){
                cf_sock_cli = cf_allocator_simple_alloc(sizeof(cf_socket));
                cf_sock_cli->instance = (void*)(uintptr_t)sock_cli;
                cf_sock_cli->inf = &sock_inf;
                cf_sock_server->on_new_socket(cf_sock_server,cf_sock_cli);
                cf_list_push(cli_list,(void*)cf_sock_cli);
                if(sock_cli > max_fd)
                    max_fd = sock_cli;
            }
            else
            {
                close(sock_cli);
            }
        }
        for(struct cf_iterator iter = cf_list_begin(cli_list);!cf_iterator_is_end(&iter) && nready > 0;cf_iterator_next(&iter)){
            cf_socket* cf_sock_cli = (cf_socket*)cf_iterator_get(&iter);
            int sock_cli = (int)(uintptr_t)cf_sock_cli->instance;
            if(FD_ISSET(sock_cli,&r_set))
            {
                uint8_t buffer[SOCKET_BUFFER_SIZE];
                ssize_t count = read(sock_cli,buffer,SOCKET_BUFFER_SIZE);
                if(count <= 0)
                {
                    close(sock_cli);
                    if(cf_sock_server->on_disconnect){
                        cf_sock_server->on_disconnect(cf_sock_server,cf_sock_cli);
                    }
                    cf_iterator_remove(&iter);
                    cf_allocator_simple_free(cf_sock_cli);
                    continue;
                }
                if(cf_sock_server->on_client_read)
                    cf_sock_server->on_client_read(cf_sock_cli,buffer,count);
            }
            if(FD_ISSET(sock_cli,&except_set)){
                close(sock_cli);
                if(cf_sock_server->on_disconnect){
                    cf_sock_server->on_disconnect(cf_sock_server,cf_sock_cli);
                }
                cf_iterator_remove(&iter);
                cf_allocator_simple_free(cf_sock_cli);
            }
        }
    }
    return CF_OK;
}
// static cf_socket* pending_connection(cf_socket* cf_sock){
//     int sock = (int)(uint64_t)cf_sock->instance;
//     int sock_cli = accept(sock, NULL,NULL);
//     cf_socket* cf_sock_cli = cf_allocator_simple_alloc(sizeof(cf_socket));
//     cf_sock_cli->instance = (void*)(uint64_t)sock_cli;
//     cf_list_push(cli_list,(void*)cf_sock_cli);
//     if(sock_cli > max_fd)
//         max_fd = sock_cli;

// }
static int socket_write(cf_socket* sock,const uint8_t* buf,size_t n){
    write((int)(uintptr_t)sock->instance,buf,n);
}

cf_socket* cf_tcp_socket_create_linux(uint16_t port){
    cf_socket* cf_sock = (cf_socket*)cf_allocator_simple_alloc(sizeof(cf_socket));
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1)
        goto end1;
    int opt = SO_REUSEADDR;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const void *) &opt, (socklen_t)sizeof (opt));
    struct sockaddr_in addr;
    int addr_len;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr_len = sizeof (addr);
    int ret = bind(sock, (struct sockaddr *) &addr, addr_len);
    if(ret == -1)
        goto end2;
    listen(sock, 10);
    cf_sock->instance = (void*)(uintptr_t)sock;
    cf_sock->inf = &sock_inf;
    return cf_sock;
end2:
    close(sock);
end1:
    cf_allocator_simple_free(cf_sock);
    return NULL;
}
