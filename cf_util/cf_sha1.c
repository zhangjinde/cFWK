#include <stdint.h>
#include <stddef.h>
#include "SHA1.h"
void cf_sha1_generate(uint8_t* src,uint8_t* sha1,size_t len){
    SHA1Context sha;
    SHA1Reset(&sha);
    SHA1Input(&sha,src,len);
    SHA1Result(&sha, sha1);
}