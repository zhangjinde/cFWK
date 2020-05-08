#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "cf_threadpool/cf_threadpool.h"
#include "cf_allocator/cf_allocator_simple.h"
#include "cf_async_queue/cf_async_queue.h"
#include "cf_iostream/cf_iostream_file.h"
#include "cf_iostream/cf_iostream_tcp.h" 
#include "cf_logger/cf_logger.h"
#include "cf_stp/cf_stp.h"
#include "assert.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "cf_threadpool/cf_threadpool.h"
#include "cf_std.h"
static struct cf_json* stp_server_query_opt(struct cf_stp_context* c,struct cf_json* j)
{
    cf_unused(c);
    cf_unused(j);
    struct cf_json* json = cf_json_create_object();
    cf_json_add_string_to_object(json,"mcu-ver", "app197004201920");
    cf_json_add_string_to_object(json,"fpga-ver","app197005210918");
    cf_json_add_string_to_object(json,"dev","pc");
    cf_json_add_int_to_object(json,"slot-id",16);
    return json;
}

static struct cf_json* proccessor(struct cf_stp_context* context,struct cf_json*json){
    cf_unused(context);
    printf("%s\n",cf_json_print(json));
    struct cf_json* reply = cf_json_create_object();
    cf_json_add_string_to_object(reply,"ack","this ok");
    return reply;
}
static void stp_server_test(void* d){
    cf_unused(d);
    //struct cf_stp_server* server = cf_stp_server_create(9851,"224.0.10.200",8888);
    struct cf_stp_server* server = cf_stp_server_create(9851,"255.255.255.255",8888);
    struct cf_json* multicast_msg = cf_json_create_object();

    cf_json_add_string_to_object(multicast_msg,"mcu-ver", "app197004201920");
    cf_json_add_string_to_object(multicast_msg,"fpga-ver","app197005210918");
    cf_json_add_string_to_object(multicast_msg,"dev","pc");
    cf_json_add_int_to_object(multicast_msg,"slot-id",16);
    cf_stp_server_set_multicast_msg(server,multicast_msg);
    cf_stp_server_listen(server,"test-topic",proccessor);
    cf_stp_server_listen(server,"query",stp_server_query_opt);
    cf_stp_server_run(server);
    cf_stp_server_destroy(server);
}
static void stp_client_multi_test(void* d){
    cf_unused(d);
    struct cf_stp_client* client = cf_stp_client_create("255.255.255.255",8888);
    while(true){
        struct cf_json* json = cf_stp_client_recv_multicast(client);
        if(json)
        {
            printf("%s\n",cf_json_print(json));
            cf_json_destroy_object(json);
        }
    }
    cf_stp_client_destroy(client);
}
static void stp_client_unicast_test(void* d){
    struct cf_json* msg = NULL;
    struct cf_json* ack = NULL;
    const char* file_name = (char*)d;
    struct cf_stp_client* client = cf_stp_client_create(NULL,0);
    int ret = cf_stp_client_connect(client,"127.0.0.1",9851);
    if(ret != 0 )
    {
        perror("client connect error: ");
        cf_stp_client_destroy(client);
        return;
    }
    if( file_name){
        printf("filename=%s\n",file_name);
        struct stat statbuf;
        stat(file_name,&statbuf);
        int size=statbuf.st_size;

        msg = cf_json_create_object();
        cf_json_add_string_to_object(msg,"file-name","./test_file");
        cf_json_add_int_to_object(msg,"file-len",size);
        ack = cf_stp_client_request(client,"file-upload",msg);
        if(ack && strcmp(cf_json_get_string(cf_json_get_item(ack,"reply"),"ack",NULL),"ok") == 0 )
        {
            FILE* fd = fopen(file_name,"rb");
            if(fd)
            {
                uint8_t buff[102400];
                
                while(size > 0){
                    size_t len = size > sizeof(buff) ? sizeof(buff) : size;
                    ssize_t count = fread(buff,1,len,fd);
                    printf("read file %ld byte\n",count);
                    if(count != len)
                    {
                        printf("xxx\n");
                    }
                    cf_stp_client_write_binary(client,buff,count);
                    size-=count;
                }
                fclose(fd);
            }
        }
        cf_json_destroy_object(msg);
        msg = NULL;
        if(ack){
            cf_json_destroy_object(ack);
            ack = NULL;
        }
            
        
    }
    while(true){
        struct cf_json* msg = cf_json_create_object();
        cf_json_add_string_to_object(msg,"test-string","hello");
        struct cf_json* json = cf_stp_client_request(client,"test-topic",msg);
        if(json)
        {
            printf("%s\n",cf_json_print(json));
            cf_json_destroy_object(json);
        }
        cf_json_destroy_object(msg);
        sleep(10);
    }

}


int test_stp(){
    cf_threadpool_run(stp_server_test,NULL);
    cf_threadpool_run(stp_client_multi_test,NULL);
    sleep(1);
    const void* d = NULL;
    const char* test_transfer_file = "cf_stp/cf_stp_test";
    
    cf_threadpool_run(stp_client_unicast_test,( void*)test_transfer_file);
    while(true){
        sleep(1);
        printf("alloc_size=%ld\n",cf_allocator_alloc_size());
    }
    return 0;
}