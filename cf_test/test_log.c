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
#include "assert.h"

#define TEST_TCP_STREAM_PORT 10350

static void tcp_iostresm_server_test(void* d){
    int* state = (int*)d;
	struct sockaddr_in serv_addr, clie_addr;
	socklen_t clie_addr_len;
	int sfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(sfd >= 0);
	bzero(&serv_addr, sizeof(serv_addr));      
	serv_addr.sin_family = AF_INET;   
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//仅本地
	serv_addr.sin_port = htons(TEST_TCP_STREAM_PORT);              

	bind(sfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	listen(sfd, 64);
	printf("wait for client connect ...\n");
	clie_addr_len = sizeof(clie_addr);
	//阻塞，等待客户端发起连接
	int cfd = accept(sfd, (struct sockaddr *)&clie_addr, &clie_addr_len);

    char buf[1024];
	while (*state == 0) {
		//循环读取客户端的数据请求
        uint32_t size = 0;
		int len = read(cfd, &size, sizeof(size));
        //read返回0说明对端已经关闭
		if(len < sizeof(size) || len > sizeof(buf))
		{
            printf("连接关闭，服务器退出\n");
            close(cfd);
			break;
		}
        uint32_t recv_bytes = 0;
        while(recv_bytes < size){
            len = read(cfd, &buf, size - recv_bytes);
            if(len <= 0){
                close(cfd);
                cfd = -1;
                break;
            }
            recv_bytes += len;
        }
        if(recv_bytes < size){
            printf("接收错误\n");
            break;
        }
        printf("server: %s",buf);
	}
	close(sfd);
    *state = 2;
	return ;
}
int test_log(){
    
    //cf_threadpool_run(test_loop_print,NULL);
    cf_log(NULL,CF_LOG_DEBUG,"%d.hello first log just console\n",1);
    cf_iostream* out = cf_iostream_from_file("./test_file_stream_out.txt",CF_IOS_WRITE);
    cf_iostream_writeln(out,"2.test write file outstream\n");
    cf_log_add_out(NULL,out);
    cf_log(NULL,CF_LOG_DEBUG,"%d.log out put std-file\n",3);
    cf_log_remove_out(NULL,out);    //会自动释放out资源
    cf_log(NULL,CF_LOG_DEBUG,"%d.log out put std\n",4);
    cf_log_level default_level = cf_log_set_level(NULL,CF_LOG_ERROR);
    cf_log(NULL,CF_LOG_DEBUG,"%d.log out put std\n",5);
    cf_log_set_level(NULL,default_level);
    cf_log(NULL,CF_LOG_DEBUG,"%d.log out put std\n",6);

    int state = 0;
    cf_threadpool_run(tcp_iostresm_server_test,&state);
    usleep(1000);
    cf_iostream* tcp_out = cf_iostream_tcp_connect("127.0.0.1",TEST_TCP_STREAM_PORT);
    assert(tcp_out != NULL);
    cf_iostream_writeln(tcp_out,"7.test write tcp outstream\n");
    cf_log_add_out(NULL,tcp_out);
    cf_log(NULL,CF_LOG_DEBUG,"%d.log out put std\n",8);
    usleep(3000);
    state = 1;
    cf_log_remove_out(NULL,tcp_out);
    while(state != 2)
    {
        usleep(10000);
    }
    printf("test_log finish\n");
    return 0;
}