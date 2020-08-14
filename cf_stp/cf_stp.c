#include <stdint.h>
#include <unistd.h>
#include<stdio.h>
#include<stdlib.h>  
#include<string.h>     
#include<unistd.h>      
#include<sys/types.h>    
#include<sys/socket.h>     
#include<arpa/inet.h>  
#include<sys/select.h>   
#include<netinet/in.h>

#include "cf_stp.h"
#include "cf_allocator/cf_allocator_simple.h"
#include "cf_collection/cf_hash.h"
#include "cf_collection/cf_vector.h"
#include "cf_util/cf_util.h"
 

#include "cf_collection/cf_list.h"
#include "cf_json/cf_json.h"

#define STP_MAX_PKG_SIZE    (60*1024)
// struct _remote_point
// {
//     struct sockaddr_in m_addr;
//     int m_len;
// };

// static struct _remote_point _remote_point_init(struct _remote_point* rp,const char* ip_addr,uint16_t port)
// {
//     rp->m_addr.sin_family = AF_INET;
//     rp->m_addr.sin_addr.s_addr = inet_addr(ip_addr);
//     rp->m_addr.sin_port = htons(port);
//     rp->m_len = sizeof(m_addr);
// }
struct cf_stp_server{
    int m_multicast_socket;
    int m_server_socket;
    struct cf_hash* m_processors;
    struct cf_json* m_multicast_msg;
    uint32_t m_multi_addr;
    uint16_t m_multi_port;
    uint16_t m_server_port;
};
struct cf_stp_context{
    int m_socket;
    struct in_addr m_cli_addr;
    FILE* m_fd;   //用于文件传输模式
    size_t m_file_len;//传输文件长度
    size_t m_file_offset;//传输文件当前字节
};
struct cf_stp_client{
    int m_multicast_socket;
    int m_cli_socket;
    uint32_t m_seq;
};
void cf_stp_client_deinit(struct cf_stp_client* client){
    if(client->m_multicast_socket >= 0)
        close(client->m_multicast_socket);
    if(client->m_cli_socket >= 0)
        close(client->m_cli_socket);
    return;
}
void cf_stp_client_destroy(struct cf_stp_client* client){
    cf_stp_client_deinit(client);
    cf_allocator_simple_free(client);
}
int cf_stp_client_init(struct cf_stp_client* client,const char* multicast_addr ,uint16_t multicast_port)
{
    int err;
    struct sockaddr_in local_addr;
    client->m_multicast_socket = -1;
    client->m_cli_socket = -1;
    client->m_seq = 1;
    if( multicast_addr){
        client->m_multicast_socket = socket(AF_INET, SOCK_DGRAM, 0);
        if (client->m_multicast_socket == -1)
        {
            perror("socket()");
            goto err1;
        }
        
        memset(&local_addr, 0, sizeof(local_addr));
        local_addr.sin_family = AF_INET;
        local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        local_addr.sin_port = htons(multicast_port);

        
        /*绑定socket*/
        int opt = 1;
        setsockopt(client->m_multicast_socket,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt)); //使能地址重用模式
        err = bind(client->m_multicast_socket,(struct sockaddr*)&local_addr, sizeof(local_addr)) ;
        if(err < 0)
        {
            close(client->m_multicast_socket);
            client->m_multicast_socket = -1;
            perror("bind client");
            goto err1;
        }

        if(strcmp(multicast_addr,"255.255.255.255") == 0 ) //广播
        {

            bool optval=true;
            setsockopt(client->m_multicast_socket,SOL_SOCKET,SO_BROADCAST,(char*)&optval,sizeof(bool));
        }
        else //组播
        {
            struct ip_mreq mreq;                                /*加入多播组*/
            mreq.imr_multiaddr.s_addr = inet_addr(multicast_addr);  /*多播地址*/
            mreq.imr_interface.s_addr = htonl(INADDR_ANY);      /*网络接口为默认*/
            /*将本机加入多播组*/
            err = setsockopt(client->m_multicast_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP,&mreq, sizeof(mreq));
        }
        
        
        if (err < 0)
        {
            perror("setsockopt():IP_ADD_MEMBERSHIP");
            goto err2;
        }
    }
    client->m_cli_socket = socket(AF_INET, SOCK_STREAM, 0);         /*建立套接字*/
    if (client->m_cli_socket == -1)
    {
        perror("socket()");
        goto err2;
    }
    
    return 0;

err2:
    if(client->m_multicast_socket >= 0)
    {
        close(client->m_multicast_socket);
        client->m_multicast_socket = -1;
    }
err1:
    return -1;

}
struct cf_stp_client*  cf_stp_client_create(const char* multicast_addr ,uint16_t multicast_port)
{
    struct cf_stp_client* client = (struct cf_stp_client*)cf_allocator_simple_alloc(sizeof(struct cf_stp_client));
    int ret = cf_stp_client_init(client,multicast_addr,multicast_port);
    if(ret < 0)
    {
        cf_allocator_simple_free(client);
        client = NULL;
    }
    return client;
}
struct cf_json* cf_stp_client_recv_multicast(struct cf_stp_client* client){
    if(client->m_multicast_socket < 0)
        return NULL;
    char buffer[CF_STP_CLIENT_RECV_BUFF_SIZE];
    struct sockaddr_in local_addr;
    socklen_t sock_len = sizeof(local_addr);
    ssize_t count = recvfrom(client->m_multicast_socket,buffer,sizeof(buffer),0,(struct sockaddr *)&local_addr,&sock_len);
    //ssize_t count = recv(client->m_multicast_socket,buffer,sizeof(buffer),0);
    if(count < 0)
        return NULL;
    struct cf_json* recv_json = cf_json_load(buffer);
    cf_json_add_string_to_object(recv_json,"server-ip",inet_ntoa(local_addr.sin_addr));
    return recv_json;
}
int cf_stp_client_connect(struct cf_stp_client* client,const char* ipaddr,uint16_t port){
    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = inet_addr(ipaddr);
    local_addr.sin_port = htons(port);
    return connect(client->m_cli_socket,(struct sockaddr *)&local_addr,sizeof(local_addr));
}

struct cf_json* cf_stp_client_request(struct cf_stp_client* client,const char* topic,struct cf_json* msg){
    static struct cf_vector* byte_vector = NULL;
    if(byte_vector == NULL)
    {
        byte_vector = cf_vector_create(1,0);
    }
    struct cf_json* request = cf_json_create_object();
    cf_json_add_int_to_object(request,"seq",client->m_seq++);
    cf_json_add_string_to_object(request,"topic",topic);
    if(msg != NULL)
        cf_json_add_item_to_object(request,"msg",cf_json_clone(msg));
    const char* request_str = cf_json_print(request);
    cf_vector_resize(byte_vector,strlen(request_str)+sizeof(uint32_t)+sizeof(uint32_t)+5);
    *(uint32_t*)cf_vector_buffer(byte_vector) = cf_vector_length(byte_vector)-sizeof(uint32_t);
    *((uint8_t*)cf_vector_buffer(byte_vector)+sizeof(uint32_t)) = 0;
    strcpy((char*)cf_vector_buffer(byte_vector)+sizeof(uint32_t)+sizeof(uint32_t),request_str);
    ssize_t count = write(client->m_cli_socket,cf_vector_buffer(byte_vector),cf_vector_length(byte_vector));
    cf_json_destroy_object(request);
    if(count < cf_vector_length(byte_vector))
    {
        return NULL;
    }
    uint32_t pendings_size = 0;
    count = read(client->m_cli_socket,&pendings_size,sizeof(pendings_size));
    if(count < sizeof(pendings_size))
    {
        return NULL;
    }
    cf_vector_resize(byte_vector,pendings_size);
    char* ptr = (char*)cf_vector_buffer(byte_vector);
    do{
        count = read(client->m_cli_socket,ptr,pendings_size);
        if(count < 0)
        {
            break;
        }
        else
        {
            ptr += count;
            pendings_size -= count;
        }
    }while(pendings_size > 0);
    if(pendings_size > 0)
    {
        return NULL;
    }
    struct cf_json* response = cf_json_load((char*)cf_vector_buffer(byte_vector));
    // struct cf_json* response_reply = cf_json_detach_item(response,"reply");
    // cf_json_destroy_object(response);
    return response;
}

int cf_stp_client_write_binary(struct cf_stp_client* client,const uint8_t* data,size_t data_len){
    static struct cf_vector* byte_vector = NULL;
    if(byte_vector == NULL)
    {
        byte_vector = cf_vector_create(1,0);
    }
    cf_vector_resize(byte_vector,data_len+sizeof(uint32_t)+sizeof(uint32_t));

    *(uint32_t*)cf_vector_buffer(byte_vector) = cf_vector_length(byte_vector)-sizeof(uint32_t);
    *((uint8_t*)cf_vector_buffer(byte_vector)+sizeof(uint32_t)) = 1;
    memcpy((uint8_t*)cf_vector_buffer(byte_vector)+sizeof(uint32_t)+sizeof(uint32_t),data,data_len);
    ssize_t count = write(client->m_cli_socket,cf_vector_buffer(byte_vector),cf_vector_length(byte_vector));
    if(count < cf_vector_length(byte_vector))
    {
        return -1;
    }
    return 0;
}

static struct cf_json* stp_server_file_upload_opt(struct cf_stp_context* context,struct cf_json* msg)
{
    char* file_name = cf_json_get_string(msg,"file-name",NULL);
    context->m_fd = fopen(file_name,"w");
    struct cf_json* ack = cf_json_create_object();
    if(context->m_fd == NULL)
    {
        cf_json_add_string_to_object(ack,"ack","error");
    }
    else
    {
        cf_json_add_string_to_object(ack,"ack","ok");
        context->m_file_offset = 0;
        context->m_file_len = cf_json_get_int(msg,"file-len",NULL);
    }
    return ack;
}
int cf_stp_server_init(struct cf_stp_server* server,uint16_t port ,const char* multicast_addr ,uint16_t multicast_port)
{
    int err;
    int opt = 1;
    server->m_multicast_socket = -1;
    server->m_server_socket = -1;
    server->m_multi_addr = 0;
    server->m_multi_port = multicast_port;
    server->m_server_port = port;
    server->m_multicast_msg = NULL;
    

    struct sockaddr_in local_addr;
    if( multicast_addr){
        server->m_multi_addr = inet_addr(multicast_addr);
        server->m_multicast_socket = socket(AF_INET, SOCK_DGRAM, 0);
        if (server->m_multicast_socket == -1)
        {
            perror("socket()");
            goto err1;
        }

        memset(&local_addr, 0, sizeof(local_addr));
        local_addr.sin_family = AF_INET;
        local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        local_addr.sin_port = 0;
        bind(server->m_multi_addr,(struct sockaddr*)&local_addr, sizeof(local_addr)) ;


        if(server->m_multi_addr == 0xffffffff ) //广播
        {

            bool optval=true;
            setsockopt(server->m_multicast_socket,SOL_SOCKET,SO_BROADCAST,(char*)&optval,sizeof(bool));
        }
        else //组播
        {
            struct ip_mreq mreq;                                /*加入多播组*/
            mreq.imr_multiaddr.s_addr = server->m_multi_addr;  /*多播地址*/
            mreq.imr_interface.s_addr = htonl(INADDR_ANY);      /*网络接口为默认*/
            /*将本机加入多播组*/
            setsockopt(server->m_multicast_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP,&mreq, sizeof(mreq));
        }
        server->m_multicast_msg = cf_json_create_object();
        cf_json_add_string_to_object(server->m_multicast_msg,"multicast-msg","this is a test");
        
    }
    server->m_server_socket = socket(AF_INET, SOCK_STREAM, 0);         /*建立套接字*/
    if (server->m_server_socket == -1)
    {
        perror("socket()");
        goto err2;
    }
    
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    local_addr.sin_port = htons(port);

    opt = 1;
    setsockopt(server->m_server_socket,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt)); //使能地址重用模式
    err = bind(server->m_server_socket,(struct sockaddr*)&local_addr, sizeof(local_addr)) ;
    if(err < 0)
    {
        close(server->m_server_socket);
        server->m_server_socket = -1;
        perror("bind server");
        goto err2;
    }
    listen(server->m_server_socket,100);
    server->m_processors = cf_hash_create(cf_hash_str_hash,cf_hash_str_equal,NULL,NULL);

    cf_stp_server_listen(server,"file-upload",stp_server_file_upload_opt);

    return 0;

err2:
    if(server->m_multicast_socket >= 0)
    {
        close(server->m_multicast_socket);
        server->m_multicast_socket = -1;
        if(server->m_multicast_msg)
        {
            cf_json_destroy_object(server->m_multicast_msg);
            server->m_multicast_msg = NULL;
        }
    }
err1:
    return -1;
}
struct cf_stp_server* cf_stp_server_create(uint16_t port ,const char* multicast_addr ,uint16_t multicast_port)
{

    struct cf_stp_server* server = (struct cf_stp_server*)cf_allocator_simple_alloc(sizeof(struct cf_stp_server));
    int ret = cf_stp_server_init(server, port , multicast_addr , multicast_port);
    if(ret != 0)
    {
        cf_allocator_simple_free(server);
        server = NULL;
    }
    return server;
}

void cf_stp_server_deinit(struct cf_stp_server* server){
    cf_hash_delete(server->m_processors);
    if(server->m_multicast_socket >= 0)
        close(server->m_multicast_socket);
    if(server->m_server_socket >= 0)
        close(server->m_server_socket);
    if(server->m_multicast_msg)
        cf_json_destroy_object(server->m_multicast_msg);
}

void cf_stp_server_destroy(struct cf_stp_server*server)
{
    cf_stp_server_deinit(server);
    cf_allocator_simple_free(server);
}
void cf_stp_server_set_multicast_msg(struct cf_stp_server* server,struct cf_json* msg){
    cf_json_destroy_object(server->m_multicast_msg);
    server->m_multicast_msg = msg;
}
//proccessor 返回的cf_json不需要释放，会在处理流程框架自动释放
void cf_stp_server_listen(struct cf_stp_server* server,const char* topic,struct cf_json* (*proccessor)(struct cf_stp_context* ,struct cf_json*))
{
    cf_hash_insert(server->m_processors,(void*)(unsigned long)topic,(void*)proccessor); 
}

const char* cf_stp_context_get_client_ip(struct cf_stp_context* c){
    return inet_ntoa(c->m_cli_addr);
}
int cf_stp_server_run(struct cf_stp_server* server){
    struct cf_list* cli_list = cf_list_create(NULL);
    int max_fd = server->m_server_socket;
    while(true){
        fd_set r_set;
        FD_ZERO(&r_set);
        FD_SET(server->m_server_socket,&r_set);
        for(struct cf_iterator iter = cf_list_begin(cli_list);!cf_iterator_is_end(&iter);cf_iterator_next(&iter)){
            struct cf_stp_context* context = (struct cf_stp_context*)cf_iterator_get(&iter);
            FD_SET(context->m_socket,&r_set);
        }
        struct timeval timeout;
        struct timeval* p_timeout = NULL;
        static int64_t last_ms = -1;
        if(last_ms == -1)
            last_ms = cf_util_gettime_ms();
        if(server->m_multicast_socket > -1)
        {
            int64_t this_ms = cf_util_gettime_ms();
            int64_t remain_ms = 3000-(this_ms-last_ms);
            remain_ms = remain_ms < 0 ? 0 :remain_ms;
            timeout.tv_sec = remain_ms/1000;
            timeout.tv_usec = remain_ms%1000*1000;
            p_timeout = &timeout;
        }

        int nready = select(max_fd+1,&r_set,NULL,NULL,p_timeout);
        if(nready == 0 && server->m_multicast_socket >= 0)
        {
            static struct cf_json* multi_json = NULL;
            if(multi_json == NULL)
            {
                multi_json = cf_json_create_object();
                cf_json_add_item_to_object(multi_json,"msg",server->m_multicast_msg);
                cf_json_add_int_to_object(multi_json,"port",server->m_server_port);
            }
            
            struct sockaddr_in sock_addr;
            sock_addr.sin_family = AF_INET;
            sock_addr.sin_addr.s_addr = server->m_multi_addr;
            sock_addr.sin_port = htons(server->m_multi_port);
            int sock_len = sizeof(sock_addr);
            const char* json_str = cf_json_print(multi_json);
            sendto(server->m_multicast_socket,json_str,strlen(json_str)+1,0,(struct sockaddr *)&sock_addr,sock_len);
            last_ms = cf_util_gettime_ms();
        }
        if(FD_ISSET(server->m_server_socket,&r_set)){
            struct sockaddr_in cli_addr;
            socklen_t len = sizeof(cli_addr);
            int cli_sock = accept(server->m_server_socket, (struct sockaddr *)&cli_addr, &len);
            //printf("accept ... \n");
            if(cli_sock < 0)
            {
                //COMM_ERROR("accept nova protocol cli socket failure.");
            }
            else
            {
                struct cf_stp_context* context = (struct cf_stp_context*)cf_allocator_simple_alloc(sizeof(struct cf_stp_context));
                context->m_socket = cli_sock;
                context->m_cli_addr = cli_addr.sin_addr;
                context->m_fd = NULL;
                context->m_file_len = 0;
                context->m_file_offset = 0;
                cf_list_push(cli_list,(void*)context);
                //COMM_LOG("accept client %s:%hu\n",inet_ntoa(cli_addr.sin_addr),cli_addr.sin_port);
                if(cli_sock > max_fd)
                    max_fd = cli_sock;
            }
        }
        for(struct cf_iterator iter = cf_list_begin(cli_list);!cf_iterator_is_end(&iter);cf_iterator_next(&iter))
        {
            struct cf_stp_context* context = (struct cf_stp_context*)cf_iterator_get(&iter);

            if(FD_ISSET(context->m_socket,&r_set))
            {
                uint32_t pending_size = 0;
                ssize_t count = read(context->m_socket,&pending_size,sizeof(uint32_t)) ;
                if(count <= 0)
                {
                    close(context->m_socket);
                    if(context->m_fd)
                        fclose(context->m_fd);
                    cf_allocator_simple_free(context); 
                    cf_iterator_remove(&iter);
                    continue;
                }
                static struct cf_vector* byte_vector = NULL;
                if(byte_vector == NULL)
                    byte_vector = cf_vector_create(1,0);
                cf_vector_resize(byte_vector,pending_size);
                uint8_t* ptr = (uint8_t*)cf_vector_buffer(byte_vector);
                do{
                    count = read(context->m_socket,ptr,pending_size);
                    if(count <= 0)
                    {
                        close(context->m_socket);
                        if(context->m_fd)
                            fclose(context->m_fd);
                        cf_allocator_simple_free(context); 
                        cf_iterator_remove(&iter);
                        continue;
                    }
                    pending_size -= count;
                    ptr+=count;
                }while(pending_size > 0);
                if(pending_size > 0)
                    continue;

                uint8_t transfer_mod = ((uint8_t*)cf_vector_buffer(byte_vector))[0];
                if(transfer_mod == 0)//json mode
                {
                    struct cf_json* json = (struct cf_json*)cf_json_load((char*)cf_vector_buffer(byte_vector)+4);
                    if(json == NULL)
                        continue;
                    int seq = cf_json_get_int(json,"seq",NULL);
                    char* topic = cf_json_get_string(json,"topic",NULL);
                    struct cf_json* (*proccessor)(struct cf_stp_context*,struct cf_json* ) = (struct cf_json* (*)(struct cf_stp_context*,struct cf_json* ))cf_hash_get(server->m_processors,topic,NULL); 

                    struct cf_json*  reply = NULL;
                    if(proccessor){
                        reply = proccessor(context,cf_json_get_item(json,"msg"));
                    }
                    cf_json_destroy_object(json);
                    struct cf_json* respone = cf_json_create_object();
                    if(reply)
                        cf_json_add_item_to_object(respone,"reply",reply);
                    cf_json_add_int_to_object(respone,"seq",seq);
                    char* json_str = cf_json_print(respone);
                    size_t json_str_size = strlen(json_str);
                    cf_vector_resize(byte_vector,json_str_size+sizeof(uint32_t)+1);
                    ((uint8_t*)cf_vector_buffer(byte_vector))[cf_vector_length(byte_vector)-1] = 0;
                    (*(uint32_t*)cf_vector_buffer(byte_vector)) = cf_vector_length(byte_vector) - sizeof(uint32_t);
                    strcpy((char*)cf_vector_buffer(byte_vector)+sizeof(uint32_t),json_str);
                    count = write(context->m_socket,cf_vector_buffer(byte_vector),cf_vector_length(byte_vector));
                    cf_json_destroy_object(respone);
                    
                    if(count <= 0)
                    {
                        close(context->m_socket);
                        if(context->m_fd)
                            fclose(context->m_fd);
                        cf_allocator_simple_free(context); 
                        cf_iterator_remove(&iter);
                        continue;
                    }
                }
                else if( transfer_mod == 1) //file_upload_mod
                {
                    size_t byte_len = cf_vector_length(byte_vector) - 4;
                    if(context->m_fd)
                    {
                        ssize_t size = fwrite((uint8_t*)cf_vector_buffer(byte_vector)+4,1,byte_len,context->m_fd);
                        printf("write file %d bytes\n",(int)size);
                    }
                        
                    context->m_file_offset += byte_len;
                    if(context->m_file_offset == context->m_file_len && context->m_fd)
                    {
                        fclose(context->m_fd);
                        context->m_fd = NULL;
                    }
                }
            }
        }
    }
    cf_list_delete(cli_list);
}

