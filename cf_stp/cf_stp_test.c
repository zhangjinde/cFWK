/********************************
 gcc -std=c99  -g -D_GNU_SOURCE  -o cf_stp_test cf_stp.c -I../ ../cf_collection/cf_list.c ../cf_collection/cf_hash.c ../cf_collection/cf_vector.c \
  ../cf_collection/cf_iterator.c ../cf_allocator/cf_allocator_simple.c ../cf_json/cf_json.c ../cf_json/cJSON/cJSON.c ../cf_util/cf_util.c \
  ../cf_threadpool/cf_threadpool.c ../cf_async_queue/cf_async_queue.c cf_stp_test.c -lpthread 
 * ******************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "cf_json/cf_json.h"
#include "cf_threadpool/cf_threadpool.h"
#include "cf_std.h"
struct cf_json* proccessor(struct cf_stp_context* context,struct cf_json*json){
    cf_unused(context);
    printf("%s\n",cf_json_print(json));
    struct cf_json* reply = cf_json_create_object();
    cf_json_add_string_to_object(reply,"ack","this ok");
    return reply;
}
void stp_server_test(void* d){
    cf_unused(d);
    struct cf_stp_server* server = cf_stp_server_create(8098,"224.0.10.200",8888);
    struct cf_json* multicast_msg = cf_json_create_object();
    cf_json_add_string_to_object(multicast_msg,"test-multicast","123");
    cf_stp_server_set_multicast_msg(server,multicast_msg);
    cf_stp_server_listen(server,"test-topic",proccessor);
    cf_stp_server_run(server);
    cf_stp_server_destroy(server);
}
void stp_client_multi_test(void* d){
    cf_unused(d);
    struct cf_stp_client* client = cf_stp_client_create("224.0.10.200",8888);
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
void stp_client_test(void* d){
    struct cf_json* msg = NULL;
    struct cf_json* ack = NULL;
    const char* file_name = d;
    struct cf_stp_client* client = cf_stp_client_create(NULL,0);
    int ret = cf_stp_client_connect(client,"127.0.0.1",8098);
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
        sleep(1);
    }

}
int main(int argc,const char* argv[]){
    cf_threadpool_run(stp_server_test,NULL);
    //cf_threadpool_run(stp_client_multi_test,NULL);
    sleep(1);
    const void* d = NULL;
    d = "cf_stp/cf_stp_test";
    printf("argc=%d\n",argc);
    if(argc > 1)
        d = argv[1];
    
    //cf_threadpool_run(stp_client_test,( void*)d);
    while(true){
        sleep(1);
        //printf("alloc_size=%ld\n",cf_allocator_alloc_size());
    }
    return 0;
}