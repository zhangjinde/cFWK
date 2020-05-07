#include "cf_logger.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include "cf_collection/cf_list.h"
#include "cf_allocator/cf_allocator_simple.h"

#define LOG_LINE_SZ    1024

struct cf_logger{
    cf_log_level m_level;
    cf_list* m_out_list;    //cf_iostream* list
};
static cf_logger* get_root_logger(){
    static cf_logger* logger = NULL;
    if(logger == NULL){
        logger = (cf_logger*)cf_allocator_simple_alloc(sizeof(cf_logger));
        logger->m_level = CF_LOG_INFO;
        logger->m_out_list = cf_list_create(cf_iostream_destroy);
        if(logger->m_out_list == NULL){
            cf_allocator_simple_free(logger);
            logger = NULL;
        }
    }
    return logger;
}

void cf_log(struct cf_logger* logger,cf_log_level level,const char* format,...){
    if(level > logger->m_level)
        return;
    va_list args;
    va_start(args,format);
    char line[LOG_LINE_SZ];
    vsnprintf(line,sizeof(line),format, args);
    va_end(args);
    
    time_t now;
    struct tm *tm_now;
    time(&now);
    tm_now = localtime(&now);
    char time_str[64];
    snprintf(time_str,sizeof(time_str),"%02d-%02d-%02d %02d:%02d:%02d\n", (tm_now->tm_year+1900-2000), tm_now->tm_mon+1, tm_now->tm_mday,
              tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
    char line2[LOG_LINE_SZ];
    snprintf(line2,sizeof(line2),"%s--%s\n", time_str,line);
    for(cf_iterator iter = cf_list_begin(logger->m_out_list);!cf_iterator_is_end(&iter);cf_iterator_next(&iter)){
        cf_iostream_writeln((cf_iostream*)cf_iterator_get(&iter) ,line2);
    }
}



#ifdef TEST_CF_LOGGER
/*************************************
 * gcc -DTEST_CF_LOGGER cf_logger.c -o cf_logger_test
 **************************************/
int main(){
    cf_logger_root_info("INFO THIS IS A TEST %s","hello");
    cf_logger_root_dgb("DEBUG THIS IS A TEST %s","hello");
    cf_logger_root_error("ERROR THIS IS A TEST %s","hello");
    return 0;
}
#endif