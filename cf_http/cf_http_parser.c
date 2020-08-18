#include "cf_http_parser.h"
#include "cf_collection/cf_string.h"
#include "cf_allocator/cf_allocator_simple.h"
#include <stdio.h>
#include <string.h>
typedef struct cf_http_request{
    cf_string* method;
    cf_string* upgrade;
    cf_string* ws_key;
}cf_http_request;
static const char* take_line(const char* text,size_t text_len,char* line,size_t line_len,size_t* take_len){
    size_t pos = 0;
    *take_len = 0;
    while( pos < text_len && text[pos++] != '\n' );
    if(pos == text_len){
        return text;
    }
    *take_len = pos-1 < line_len ? pos-1 : line_len;
    memcpy(line,text,*take_len); 
    line[*take_len-1] = '\0';
    return text+pos;
}

cf_http_request* cf_http_parse(const uint8_t* buffer,size_t len,size_t* parsed_len){
    // char text_buffer[2048];
    // *parsed_len = 0;
    // if(len > sizeof(text_buffer)){
    //     printf("http request text is too length.\n");
    //     return NULL;
    // }
    uint8_t str_buffer[2048];
    size_t str_buff_pos = 0;
    char line[1024];
    size_t take_len = 0;
    const uint8_t* text = take_line(buffer,len,line,sizeof(line),&take_len);
    if(take_len == 0)
    {
        return NULL;
    }
    size_t pos = 0;
    char* upgrade = NULL;
    char* ws_key = NULL;
    char* method = str_buffer + str_buff_pos;
    while(line[pos++] != ' ');
    memcpy(method,line,pos-1);
    method[pos-1] = '\0';
    str_buff_pos += pos;
    
    while(*text != '\r'){
        pos = 0;
        text = take_line(text,len-(text-buffer),line,sizeof(line),&take_len);
        if(take_len == 0)
        {
            return NULL;
        }
        char* key = line;
        while(line[pos++] != ':');
        line[pos-1] = '\0';
        while(line[pos++] != ' ');
        char* val = line+pos;
        while(line[pos++] != '\n');
        line[pos-2] = '\0';
        if(0 == strcmp(key , "Upgrade")){
            upgrade = str_buffer + str_buff_pos;
            strcpy(upgrade,val);
            str_buff_pos += strlen(upgrade)+1;
        }
        else if(0 == strcmp(key , "Sec-WebSocket-Key")){
            ws_key = str_buffer + str_buff_pos;
            strcpy(ws_key,val);
            str_buff_pos += strlen(ws_key)+1;
        }

    }
    cf_http_request* request = cf_allocator_simple_alloc(sizeof(cf_http_request));
    request->method = cf_string_create_from_cstr(method);
    request->upgrade = cf_string_create_from_cstr(upgrade);
    request->ws_key = cf_string_create_from_cstr(ws_key);
    return request;
}

void cf_http_result_destroy(cf_http_request* request){
    if(request->method)
        cf_string_destroy(request->method);
    if(request->upgrade)
        cf_string_destroy(request->upgrade);
    if(request->ws_key)
        cf_string_destroy(request->ws_key);
    cf_allocator_simple_free(request);

}
const char* cf_http_request_method(cf_http_request* request){
    return cf_string_c_str(request->method);

}

const char* cf_http_request_upgrade(cf_http_request* request){
    return cf_string_c_str(request->upgrade);
}
const char* cf_http_request_ws_key(cf_http_request* request){
    return cf_string_c_str(request->ws_key);
}
