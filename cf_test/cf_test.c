#include "cf_test.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <stdarg.h>
#define HUGE_STRING_LEN	8192
#define STRING_MAX 256
#define STRING_INC		256
static void* _malloc(int len){
    return malloc(len);
}
static void _free(void* m){
    free(m);
}
static void* _realloc(void *mem,int size){
    return realloc(mem,size);
}

typedef struct _string{
    int len;
    int size;
    char* buff;
}_string;
static _string* _string_create(){
    _string* s = _malloc(sizeof(sizeof(_string)));
    s->size = STRING_MAX;
    s->len = 0;
    s->buff = _malloc(STRING_MAX);
    s->buff[0] = 0;
    return s;
}
static void _string_destroy(_string* s){
    if( s == NULL) return;
    memset(s->buff,0,s->size);
    _free(s->buff);
    memset(s,0,sizeof(_string));
    _free(s);
}
static void _string_resize(_string* s, int size){
    s->buff = (char*) _realloc(s->buff,  size);
	s->size = size;
}
static void _string_append(_string* s, const char* text)
{
	int length;

	if (text == NULL) {
		text = "NULL";
	}

	length = strlen(text);
	if (s->len + length + 1 >= s->size)
		_string_resize(s, s->len + length + 1 + STRING_INC);
	s->len += length;
	strcat(s->buff, text);
}
static void _string_append_format(_string* s,const char* format, ...){
    va_list argp;
	char buf[HUGE_STRING_LEN];
	va_start(argp, format);
	vsprintf(buf, format, argp);
	va_end(argp);
	_string_append(s, buf);
}
static const char* _string_cstring(_string* s){
    return s->buff;
}

typedef struct cf_test{
    const char* name;
    _string* msg;
    void(*func)(cf_test*);
    bool failed;
    jmp_buf *jumpBuf;
}cf_test;

cf_test* cf_test_create(const char* name,void(*test_func)(cf_test*)){
    cf_test* tc = _malloc(sizeof(cf_test));
    memset(tc,0,sizeof(cf_test));
    tc->name = name;
    tc->func = test_func;
    return tc;
}
void cf_test_destroy(cf_test* tc){
    if(tc) return;
    if(tc->msg)
        _string_destroy(tc->msg);
    memset(tc,0,sizeof(cf_test));
    _free(tc);
}
const char* cf_test_msg(cf_test* tc){
    if(tc->msg)
        return _string_cstring(tc->msg);
    else
        return NULL;
}
const char* cf_test_name(cf_test* tc){
    return tc->name;
}
void cf_test_run(cf_test* tc){
    jmp_buf buf;
	tc->jumpBuf = &buf;
	if (setjmp(buf) == 0)
	{
		(tc->func)(tc);
	}
	tc->jumpBuf = 0;
}

void cf_assert(cf_test* tc,const char* file,int line,const char* msg,bool cond){
    if(cond)    return;
    tc->failed = true;
    if(tc->msg){
        _string_destroy(tc->msg);
    }
    tc->msg = _string_create();
    _string_append_format(tc->msg,"%s:%d %s",file,line,msg);
    if (tc->jumpBuf != 0) longjmp(*(tc->jumpBuf), 0);
}