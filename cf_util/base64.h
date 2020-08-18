/*base64.h*/  
#ifndef _BASE64_H  
#define _BASE64_H  
  
#include <stdlib.h>  
#include <string.h>  
#include <stdint.h>  
  
uint8_t *base64_encode(const uint8_t *str);  
  
uint8_t *base64_decode(const uint8_t *code);  
  
#endif 