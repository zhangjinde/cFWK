#ifndef CF_UTIL_H
#define CF_UTIL_H
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"{
#endif
int64_t cf_util_gettime_ms();
void cf_sha1_generate(uint8_t* src,uint8_t* sha1,size_t len);
#ifdef __cplusplus
}
#endif
#endif//CF_UTIL_H