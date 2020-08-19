#include<stdlib.h>
#include<sys/time.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "cf_util.h"
#include "cf_logger/cf_logger.h"

int64_t cf_util_gettime_ms()
{
    static long int st = -1;
    struct timeval tv;
    gettimeofday(&tv,NULL);
    if(st == -1)
        st = tv.tv_sec*1000+tv.tv_usec/1000;
    return tv.tv_sec*1000+tv.tv_usec/1000 - st;
}

void cf_print_arr(const char* title,const uint8_t* buf,size_t n){
    char str[1024*10];
    char tmp[10];

    strcpy(str,title);
    
    int i = 0;
    for(i = 0;i < n-1;i++){
        sprintf(tmp,"%02x ",buf[i]);
        strncat(str,tmp,sizeof(str));
    }
    if(i == n-1){
        sprintf(tmp,"%02x\n",buf[i]);
        strcat(str,tmp);
    }
    cf_log(NULL,CF_LOG_QUIET,"%s",str);
}

void cf_usleep(uint64_t ms){
    usleep(ms);
}

