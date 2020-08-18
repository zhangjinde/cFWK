#ifndef CF_WEBSOCKET_SERVER_H
#define CF_WEBSOCKET_SERVER_H
#include <stdint.h>
#include <stddef.h>
typedef struct cf_websocket_server cf_websocket_server;
typedef struct cf_websocket cf_websocket;
cf_websocket_server* cf_websocket_server_create(uint16_t port);
int cf_websocket_server_run(cf_websocket_server* server,void (*on_new_websocket)(cf_websocket_server* ,cf_websocket* ),void (*on_cli_read)(cf_websocket*,const uint8_t*,size_t ));
#endif//CF_WEBSOCKET_SERVER_H
