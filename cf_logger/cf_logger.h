#ifndef CF_LOGGER_H
#define CF_LOGGER_H
struct cf_logger;

#ifdef __cplusplus
extern "C"{
#endif

extern struct cf_logger* cf_logger_get_root();
void cf_logger_info(struct cf_logger* logger,const char*file,const char* func,int line,const char* format,...);
void cf_logger_dbg(struct cf_logger* logger,const char*file,const char* func,int line,const char* format,...);
void cf_logger_error(struct cf_logger* logger,const char*file,const char* func,int line,const char* format,...);
#define cf_logger_root_info(format,args...)  cf_logger_info(cf_logger_get_root(),__FILE__,__func__,__LINE__,format, #args)
#define cf_logger_root_dgb(format,args...)  cf_logger_dbg(cf_logger_get_root(),__FILE__,__func__,__LINE__,format, #args)
#define cf_logger_root_error(format,args...)  cf_logger_error(cf_logger_get_root(),__FILE__,__func__,__LINE__,format, #args)

#ifdef __cplusplus
}
#endif

#endif