#include "cf_logger.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include "cf_collection/cf_list.h"
#include "cf_allocator/cf_allocator_simple.h"
#include <pthread.h>
#define LOG_LINE_SZ    (1024*128)

typedef struct cf_logger{
    cf_log_level m_level;
    pthread_mutex_t m_out_list_mutex;
    cf_list* m_out_list;    //cf_iostream* list
}cf_logger;

static cf_logger* get_root_logger(){
    static cf_logger* logger = NULL;
    if(logger == NULL){
        logger = (cf_logger*)cf_allocator_simple_alloc(sizeof(cf_logger));
        logger->m_level = CF_LOG_INFO;
        logger->m_out_list = cf_list_create((cf_list_free_f)cf_iostream_destroy);
        if(logger->m_out_list == NULL){
            cf_allocator_simple_free(logger);
            logger = NULL;
        }
        pthread_mutex_init(&logger->m_out_list_mutex,0);
        cf_log_add_out( logger,cf_iostream_from_std_out());
    }
    return logger;
}

void cf_log_add_out(struct cf_logger* logger,cf_iostream* out){
    logger = logger == NULL ? get_root_logger(): logger;
    pthread_mutex_lock(&logger->m_out_list_mutex);
    cf_list_push(logger->m_out_list,out);
    pthread_mutex_unlock(&logger->m_out_list_mutex);
}

void cf_log_remove_out( cf_logger* logger,cf_iostream* out){
    logger = logger == NULL ? get_root_logger(): logger;
    pthread_mutex_lock(&logger->m_out_list_mutex);
    for(cf_iterator iter = cf_list_begin(logger->m_out_list);!cf_iterator_is_end(&iter);cf_iterator_next(&iter))
    {
        if(cf_iterator_get(&iter) == out){
            cf_iterator_remove(&iter);
            break;
        }
    }
    pthread_mutex_unlock(&logger->m_out_list_mutex);
}

void cf_log(struct cf_logger* logger,cf_log_level level,const char* format,...){
    logger = logger == NULL ? get_root_logger(): logger;
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
    snprintf(time_str,sizeof(time_str),"%02d-%02d-%02d %02d:%02d:%02d", (tm_now->tm_year+1900-2000), tm_now->tm_mon+1, tm_now->tm_mday,
              tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
    char line2[LOG_LINE_SZ];
    snprintf(line2,sizeof(line2),"%s--%s", time_str,line);
    pthread_mutex_lock(&logger->m_out_list_mutex);
    for(cf_iterator iter = cf_list_begin(logger->m_out_list);!cf_iterator_is_end(&iter);cf_iterator_next(&iter)){
        if(cf_iostream_writeln((cf_iostream*)cf_iterator_get(&iter) ,line2) < 0){
            cf_iterator_remove(&iter);
        }
    }
    pthread_mutex_unlock(&logger->m_out_list_mutex);
}
cf_log_level cf_log_set_level( cf_logger* logger,cf_log_level level){
    logger = logger == NULL ? get_root_logger(): logger;
    cf_log_level _level = logger->m_level;
    logger->m_level = level;
    return _level;
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