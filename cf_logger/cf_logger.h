#ifndef CF_LOGGER_H
#define CF_LOGGER_H
struct cf_logger;

extern struct cf_logger* cf_logger_get_root();
#define cf_logger_root_info(logger,format,args...)  cf_logger_info(cf_logger_get_root(),__FILE__,__func__,__LINE__,format, #args)
#define cf_logger_root_dgb(logger,format,args...)  cf_logger_dbg(cf_logger_get_root(),__FILE__,__func__,__LINE__,format, #args)

#endif