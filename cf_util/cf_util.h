#ifndef CF_UTIL_H
#define CF_UTIL_H
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"{
#endif
int64_t cf_util_gettime_ms();
void cf_print_arr(const char* title,const uint8_t* buf,size_t n);
void cf_sha1_generate(uint8_t* src,uint8_t* sha1,size_t len);
int cf_base64_encode(const uint8_t* str,uint8_t* code,size_t code_len);
int cf_base64_decode(const uint8_t* code,uint8_t* str,size_t str_len);
char* strlwr(char* str);
#ifdef __cplusplus
}
#endif
#endif//CF_UTIL_H