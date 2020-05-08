#include <pthread.h>

#include "cf_threadpool/cf_threadpool.h"
#include "cf_allocator/cf_allocator_simple.h"
#include "cf_async_queue/cf_async_queue.h"
#include "cf_iostream/cf_iostream_file.h"
#include "cf_logger/cf_logger.h"
int main(){
    //cf_threadpool_run(test_loop_print,NULL);
    cf_log(NULL,CF_LOG_DEBUG,"hello %d\n",1);
    cf_iostream* out = cf_iostream_from_file("./test_file_stream_out.txt",CF_IOS_WRITE);
    cf_iostream_writeln(out,"test file write 2\n");
    cf_log_add_out(NULL,out);
    cf_log(NULL,CF_LOG_DEBUG,"out put %d\n",3);
    cf_log_remove_out(NULL,out);
    cf_log(NULL,CF_LOG_DEBUG,"out put %d\n",4);
    cf_iostream_destroy(out);
    cf_log(NULL,CF_LOG_DEBUG,"out put %d\n",5);
    return 0;
}