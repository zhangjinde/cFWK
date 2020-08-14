#ifndef CF_WEBSOCKET_SERVER_H
#define CF_WEBSOCKET_SERVER_H
#include <stdint.h>
typedef struct cf_websocket_server cf_websocket_server;
typedef struct cf_websocket cf_websocket;
cf_websocket_server* cf_websocket_server_create(uint16_t port);
int cf_websocket_server_run(cf_websocket_server* server,void (*on_new_websocket)(cf_websocket_server* ,cf_websocket* ),void (*on_cli_read)(cf_websocket* ));
#endif//CF_WEBSOCKET_SERVER_H
