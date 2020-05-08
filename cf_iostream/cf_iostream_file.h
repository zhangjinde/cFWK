#ifndef CF_IOSTREAM_FILE_H
#define CF_IOSTREAM_FILE_H
#include "cf_iostream.h"
#include <stdint.h>
//flags 可选项: CF_IOS_READ |CF_IOS_WRITE
cf_iostream* cf_iostream_from_file(const char* file_name,uint32_t flags);
#endif//CF_IOSTREAM_FILE_H