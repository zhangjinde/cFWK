#include<stdlib.h>
#include<sys/time.h>
#include <unistd.h>
#include "cf_util.h"
#include <stdio.h>
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

static int print_str(unsigned char *str, int len)
{
    int i = 0;
    printf("str=[");
    for(i=0; i<len; i++)
    {
        printf("%02X", str[i]);
    }
    printf("], len=[%d]\n", len);
    return 0;
}

static unsigned long sha1_str_to_long(uint8_t *a)
{
    unsigned long x = 0;
    unsigned char *b = (unsigned char *)&x;

    b[0] = a[3];
    b[1] = a[2];
    b[2] = a[1];
    b[3] = a[0];

    return x;
}

int sha1_str_summ(unsigned char *str, unsigned char *summ, int len)
{
    unsigned char W[80][4];
    unsigned char Q[20] = {0x67, 0x45, 0x23, 0x01, 0xEF, 0xCD, 0xAB, 0x89, 0x98, 0xBA, 0xDC, 0xFE, 0x10, 0x32, 0x54, 0x76, 0xC3, 0xD2, 0xE1, 0xF0};
    unsigned long K[4]={0x5A827999,0x6ED9EBA1,0x8F1BBCDC,0xCA62C1D6};
    unsigned long N[5]={0, 0, 0, 0, 0};
    unsigned long A = sha1_str_to_long(Q);
    unsigned long B = sha1_str_to_long(Q+4);
    unsigned long C = sha1_str_to_long(Q+8);
    unsigned long D = sha1_str_to_long(Q+12);
    unsigned long E = sha1_str_to_long(Q+16);
    unsigned long F = 0;
    unsigned long G = 0;
    unsigned long H = 0;
    int u = len / 320;
    int i = 0;
    int j = 0;
    int k = 0;
    N[0] += A;
    N[1] += B;
    N[2] += C;
    N[3] += D;
    N[4] += E;
    for(i=0; i<u; i++)
    {
        for(j=0; j<80; j++)
        {
            memcpy(W[j], str+i*320+j*4, 4);
        }
        A = N[0];
        B = N[1];
        C = N[2];
        D = N[3];
        E = N[4];
        for(k=0; k<20; k++)
        {
            printf("A=[%08x]\n", A);
            G = sha1_str_to_long(W[k]);
            printf("W[%d]=[%08x]\n", i, G);
            F = ((B & C) | (~B & D));
            H = SAL(A,5) + F + E + G + K[0];
            E = D;
            D = C;
            C = SAL(B,30);
            B = A;
            A = H;
            printf("A=[%08x]\n", A);
        }
        for(k=20; k<40; k++)
        {
            G = sha1_str_to_long(W[k]);
            F = (B ^ C ^ D);
            H = SAL(A,5)+ F + E + G + K[1];
            E = D;
            D = C;
            C = SAL(B,30);
            B = A;
            A = H;
            printf("A=[%08x]\n", A);
        }
        for(k=40; k<60; k++)
        {
            G = sha1_str_to_long(W[k]);
            F = ((B & C) | (B & D) | (C & D));
            H = SAL(A,5)+ F + E + G + K[2];
            E = D;
            D = C;
            C = SAL(B,30);
            B = A;
            A = H;

            printf("A=[%08x]\n", A);
        }
        for(k=60; k<80; k++)
        {
            G = sha1_str_to_long(W[k]);
            F = (B ^ C ^ D);
            H = SAL(A,5)+ F + E + G + K[3];
            E = D;
            D = C;
            C = SAL(B,30);
            B = A;
            A = H;
            printf("A=[%08x]\n", A);
        }
        N[0] += A;
        N[1] += B;
        N[2] += C;
        N[3] += D;
        N[4] += E;
    }
    sha1_long_to_str(N[0], Q);
    sha1_long_to_str(N[1], Q+4);
    sha1_long_to_str(N[2], Q+8);
    sha1_long_to_str(N[3], Q+12);
    sha1_long_to_str(N[4], Q+16);
    memcpy(summ, Q, 20);
    return 0;
}

void cf_sha1_generate(uint8_t* src,uint8_t* sha1,size_t len){

}