#include "base64.h"  
#include "cf_std.h"
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>   
int cf_base64_encode(const uint8_t* str,uint8_t* code,size_t code_len){
    uint8_t* _code = base64_encode(str);
    strncpy((char*)code,(char*)_code,code_len-1);
    free(_code);
    code[code_len-1] = '\0';
    return CF_OK;
}
int cf_base64_decode(const uint8_t* code,uint8_t* str,size_t str_len){
    uint8_t* _str = base64_decode(code);
    strncpy((char*)str,(char*)_str,str_len-1);
    free(_str);
    str[str_len-1] = 0;
    return CF_OK;
}
