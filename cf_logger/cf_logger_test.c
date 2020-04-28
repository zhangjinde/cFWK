#include "cf_logger.h"
int main(){
    cf_logger_root_info("INFO THIS IS A TEST %s","hello");
    cf_logger_root_dgb("DEBUG THIS IS A TEST %s","hello");
    cf_logger_root_error("ERROR THIS IS A TEST %s","hello");
    return 0;
}
