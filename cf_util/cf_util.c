#include<stdlib.h>
#include<sys/time.h>
#include <unistd.h>
#include "cf_util.h"


int64_t cf_util_gettime_ms()
{
    static long int st = -1;
    struct timeval tv;
    gettimeofday(&tv,NULL);
    if(st == -1)
        st = tv.tv_sec*1000+tv.tv_usec/1000;
    return tv.tv_sec*1000+tv.tv_usec/1000 - st;
}
void cf_usleep(uint64_t ms){
    usleep(ms);
}

