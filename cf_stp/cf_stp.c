#ifndef  STPMANAGER_H
#define  STPMANAGER_H
#include "cf_stp.h"
#include "cf_collection/cf_hash.h"
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
struct stp_server{
    enum cf_stp_type_e m_type;
    int m_multicast_socket;
    int m_server_socket;
    struct cf_hash* m_processors;
};

int stp_server_init(struct stp_server* server,uint16_t port ,const char* multicast_addr ,uint16_t multicast_port)
{
    int err;
    struct sockaddr_in local_addr;

    server->m_multicast_socket = -1;
    server->m_server_socket = -1;
    if( multicast_addr){
        server->m_multicast_socket = socket(AF_INET, SOCK_DGRAM, 0);
        if (server->m_multicast_socket == -1)
        {
            perror("socket()");
            goto err1;
        }
        
        memset(&local_addr, 0, sizeof(local_addr));
        local_addr.sin_family = AF_INET;
        local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        local_addr.sin_port = htons(multicast_port);

        /*绑定socket*/
        err = bind(server->m_multicast_socket,(struct sockaddr*)&local_addr, sizeof(local_addr)) ;
        if(err < 0)
        {
            close(server->m_multicast_socket);
            server->m_multicast_socket = -1;
            perror("bind()");
            goto err1;
        }
        #if 0
        struct ip_mreq mreq;                                /*加入多播组*/
        mreq.imr_multiaddr.s_addr = inet_addr(multicast_addr);  /*多播地址*/
        mreq.imr_interface.s_addr = htonl(INADDR_ANY);      /*网络接口为默认*/
        /*将本机加入多播组*/
        err = setsockopt(socket_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP,&mreq, sizeof(mreq));
        if (err < 0)
        {
            perror("setsockopt():IP_ADD_MEMBERSHIP");
            goto err2;
        }
        #endif
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

    /*绑定socket*/
    err = bind(server->m_server_socket,(struct sockaddr*)&local_addr, sizeof(local_addr)) ;
    if(err < 0)
    {
        close(server->m_server_socket);
        server->m_server_socket = -1;
        perror("bind()");
        goto err2;
    }
    server->m_processors = cf_hash_create(cf_hash_str_hash,cf_hash_str_equal,NULL,NULL);
    return 0;

err2:
    if(server->m_multicast_socket >= 0)
    {
        close(server->m_multicast_socket);
        server->m_multicast_socket = -1;
    }
err1:
    return -1;
}
void cf_stp_server_listen_topic(struct stp_server* server,const char* topic,void(*proccessor)())
{
    cf_hash_insert(server->m_processors,topic,proccessor); 
}
void cf_stp_server_deinit(struct stp_server* server){
    cf_hash_delete(server->m_processors);
    if(server->m_multicast_socket >= 0)
        close(server->m_multicast_socket);
    if(server->m_server_socket >= 0)
        close(server->m_server_socket);
}

int cf_stp_server_run(struct stp_server* server){
    struct cf_list* cli_list = cf_list_create(NULL);
    int max_fd = server->m_server_socket;
    while(true){
        fd_set r_set;
        FD_ZERO(&r_set);
        FD_SET(server->m_server_socket,&r_set);
        for(struct cf_iterator iter = cf_list_begin(cli_list);!cf_iterator_is_end(&iter);cf_iterator_next(&iter)){
            FD_SET((int)cf_iterator_get(&iter),&r_set);
        }
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        int nready = select(max_fd+1,&r_set,NULL,NULL,&timeout);
        if(FD_ISSET(server->m_server_socket,&r_set)){
            struct sockaddr_in cli_addr;
            socklen_t len = sizeof(cli_addr);
            int cli_sock = accept(server->m_server_socket, (struct sockaddr *)&cli_addr, &len);
            //COMM_DEBUG("accept ... \n");
            if(cli_sock < 0)
            {
                //COMM_ERROR("accept nova protocol cli socket failure.");
            }
            else
            {
                cf_list_push(cli_list,cli_sock);
                //COMM_LOG("accept client %s:%hu\n",inet_ntoa(cli_addr.sin_addr),cli_addr.sin_port);
                if(cli_sock > max_fd)
                    max_fd = cli_sock;
            }

        }
        for(struct cf_iterator iter = cf_list_begin(cli_list);!cf_iterator_is_end(&iter);cf_iterator_next(&iter))
        {
            int fd = (int)cf_iterator_get(&iter);
            char buf[1024];
            if(fd,&r_set)
            {
                ssize_t count = read(fd,buf,sizeof(buf)) ;
                if(count <= 0)
                {
                    close(fd);
                    cf_iterator_remove(&iter);
                    continue;
                }

                struct cf_json* json = cf_json_load(buf);
                if(json == NULL)
                    continue;
                int seq = cf_json_get_int(json,"seq",NULL);
                char* topic = cf_json_get_string(json,"topic",NULL);
                struct cf_json* (*proccessor)(struct cf_json* ) = cf_hash_get(server->m_processors,topic,NULL); 
                struct cf_json* reply = proccessor(cf_json_get_item(json,"msg"));
                struct cf_json* respone = cf_json_create_object();
                if(reply)
                    cf_json_add_item_to_object(respone,"reply",reply);
                cf_json_add_int_to_object(respone,"seq",seq);
                
            }
        }
    }
    cf_list_delete(cli_list);
}

class STPRemotePoint{    
    struct sockaddr_in m_addr;
    int m_len;
public:
    STPRemotePoint(){
    }
    STPRemotePoint(const char* ip_addr,uint16_t port){
        m_addr.sin_family = AF_INET;
        m_addr.sin_addr.s_addr = inet_addr(ip_addr);
        m_addr.sin_port = htons(port);
        m_len = sizeof(m_addr);
    }
    friend class STPManager;
};

class STPManager{
    int m_socket;
    const int m_timeout_ms;
    int m_request_seq;
    int m_response_seq;
    bool m_is_server;
    STPManager(int socket,bool is_server = false):m_socket(socket),m_timeout_ms(3000),m_request_seq(0),m_response_seq(0),m_is_server(is_server){
        struct timeval timeout = {m_timeout_ms/1000,m_timeout_ms%1000 };
        setsockopt(socket,SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout,sizeof(struct timeval));
    }
    static STPManager* createInstance(bool is_server,uint16_t port = 0,const char* multicast_addr = nullptr){
        int err;
        int socket_fd;
        socket_fd = socket(AF_INET, SOCK_DGRAM, 0);         /*建立套接字*/
        if (socket_fd == -1)
        {
            perror("socket()");
            goto err1;
        }
        if(port != 0)
        {
            struct sockaddr_in local_addr;
            memset(&local_addr, 0, sizeof(local_addr));
            local_addr.sin_family = AF_INET;
            local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
            local_addr.sin_port = htons(port);

            /*绑定socket*/
            err = bind(socket_fd,(struct sockaddr*)&local_addr, sizeof(local_addr)) ;
            if(err < 0)
            {
                perror("bind()");
                goto err2;
            }
        }
        else
        {
            struct timeval timeout = {3,0 };
            setsockopt(socket_fd,SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout,sizeof(struct timeval));
        }

        /*设置回环许可*/
#if 0
        int loop = 1;
        err = setsockopt(socket_fd,IPPROTO_IP, IP_MULTICAST_LOOP,&loop, sizeof(loop));
        if(err < 0)
        {
            perror("setsockopt():IP_MULTICAST_LOOP");
            goto err2;
        }
#endif

        if( multicast_addr != nullptr){
            struct ip_mreq mreq;                                /*加入多播组*/
            mreq.imr_multiaddr.s_addr = inet_addr(multicast_addr);  /*多播地址*/
            mreq.imr_interface.s_addr = htonl(INADDR_ANY);      /*网络接口为默认*/
            /*将本机加入多播组*/
            err = setsockopt(socket_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP,&mreq, sizeof(mreq));
            if (err < 0)
            {
                perror("setsockopt():IP_ADD_MEMBERSHIP");
                goto err2;
            }
        }
        return new STPManager(socket_fd,is_server);
err2:
        close(socket_fd);
err1:
        return nullptr;
    }
public:
    static STPManager* createServerInstance(uint16_t port = 0,const char* multicast_addr = nullptr){
        return createInstance(true,port,multicast_addr);
    }
    static STPManager* createClientInstance(uint16_t port = 0,const char* multicast_addr = nullptr){
        return createInstance(false,port,multicast_addr);
    }
    int recv(Json::Value& jsonObj,STPRemotePoint* remote = nullptr){
        struct sockaddr_in addr_client;
        int len=sizeof(addr_client);
        char recv_buf[STP_MAX_PKG_SIZE];
        bool res = false;
        while(true){
            int recv_num = recvfrom(m_socket, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&addr_client, (socklen_t *)&len);
    //        printf("recv from recv_num=%d\n",recv_num);
            if(recv_num > 0){
                printf("recv str=%s\n",recv_buf);
                res =  JsonUtil::ParseJson(recv_buf,&jsonObj);
                if(res && remote != nullptr)
                {
                    remote->m_addr = addr_client;
                    remote->m_len = len;
                }
                if(res == true ){
                    if(m_is_server == false && jsonObj["__meta__"]["response"]["seq"] != m_request_seq)
                    {
                        res = false;
                        continue;
                    }
                    else if(m_is_server == true)
                    {
                        m_response_seq = jsonObj["__meta__"]["request"]["seq"].asInt();

                    }
                }
            }
            break;
        }
        return res == false ? -1 : 0;
    }
    int recv(Json::Value& jsonObj,int timeout_ms){
        struct timeval timeout = {timeout_ms/1000,timeout_ms%1000*1000 };
        setsockopt(m_socket,SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout,sizeof(struct timeval));
        int res = recv(jsonObj);
        timeout = {m_timeout_ms/1000,m_timeout_ms%1000*1000 };
        setsockopt(m_socket,SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout,sizeof(struct timeval));
        return res;
    }
    /*************************
     *
     * return:
     *  -1 failured
     *  -2 json string to length
     **************************/
    int send( Json::Value& jsonObj,const STPRemotePoint *remote ){
        if(remote == nullptr)
            return -1;


        if(m_is_server)
        {
            jsonObj["__meta__"]=Json::Value();
            jsonObj["__meta__"]["response"] = Json::Value();
            jsonObj["__meta__"]["response"]["seq"] = m_response_seq;
        }
        else
        {
            jsonObj["__meta__"]=Json::Value();
            jsonObj["__meta__"]["request"] = Json::Value();
            jsonObj["__meta__"]["request"]["seq"] = ++m_request_seq;
        }

        std::string str = JsonUtil::JsonToString(jsonObj);

        int res = sendto(m_socket,str.c_str(),str.length()+1,0,(const struct sockaddr*)&remote->m_addr,remote->m_len);
        printf("send to str=%s ,len=%d, res=%d\n",str.c_str(),(int)str.length()+1,res);
        if(res <= 0)
            return -1;
        else
            return 0;
    }
    int send( Json::Value& jsonObj,const STPRemotePoint& remote ){
        return send(jsonObj,&remote);
    }
    ~STPManager(){
        close(m_socket);
        m_socket = 0;
    }

};
     

#endif//  STPMANAGER_H
