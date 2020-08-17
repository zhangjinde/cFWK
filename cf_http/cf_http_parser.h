#ifndef CF_HTTP_PARSER_H
#define CF_HTTP_PARSER_H
#include <stdint.h>
#include <stddef.h>


typedef struct cf_http_request cf_http_request;
cf_http_request* cf_http_parse(uint8_t* buffer,size_t len,size_t* parsed_len);
void cf_http_result_destroy(cf_http_request* request);
char* cf_http_request_method(cf_http_request* request);

char* cf_http_request_upgrade(cf_http_request* request);
char* cf_http_request_ws_key(cf_http_request* request);


#endif//CF_HTTP_PARSER_H