#ifndef CF_LOGGER_H
#define CF_LOGGER_H
#include "cf_iostream/cf_iostream.h"

#ifdef __cplusplus
extern "C"{
#endif
typedef enum cf_log_level{
    CF_LOG_QUIET,
    CF_LOG_ERROR,
    CF_LOG_DEBUG,
    CF_LOG_INFO
}cf_log_level;
typedef struct cf_logger cf_logger;
//extern struct cf_logger* cf_logger_root();
void cf_log(struct cf_logger* logger,cf_log_level level,const char* format,...);
//void cf_log(struct cf_logger* logger,const char*file,const char* func,int line,const char* format,...);
//void cf_logger_dbg(struct cf_logger* logger,const char*file,const char* func,int line,const char* format,...);
//void cf_logger_error(struct cf_logger* logger,const char*file,const char* func,int line,const char* format,...);
//#define cf_Log_g(level,format,args...)  cf_log(cf_logger_root(),format, #args)
//#define cf_logger_root_dgb(format,args...)  cf_logger_dbg(cf_logger_root(),__FILE__,__func__,__LINE__,format, #args)
//#define cf_logger_root_error(format,args...)  cf_logger_error(cf_logger_root(),__FILE__,__func__,__LINE__,format, #args)

#ifdef __cplusplus
}
#endif

#endif