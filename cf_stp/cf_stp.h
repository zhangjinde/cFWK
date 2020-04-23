#ifndef CF_STP_H
#define CF_STP_H
#include <stdint.h>
#include <stddef.h>
#include "cf_json/cf_json.h"

#define CF_STP_SERVER_RECV_BUFF_SIZE    256
#define CF_STP_CLIENT_RECV_BUFF_SIZE    256

struct cf_stp_context;
struct cf_stp_server* cf_stp_server_create(uint16_t port ,const char* multicast_addr ,uint16_t multicast_port);
void cf_stp_server_listen(struct cf_stp_server* server, char* topic,struct cf_json* (*proccessor)(struct cf_stp_context* ,struct cf_json*));
void cf_stp_server_set_multicast_msg(struct cf_stp_server* server,struct cf_json* msg);
int cf_stp_server_run(struct cf_stp_server* server);
void cf_stp_server_destroy(struct cf_stp_server*server);

struct cf_stp_client*  cf_stp_client_create(const char* multicast_addr ,uint16_t multicast_port);
int cf_stp_client_connect(struct cf_stp_client* client,const char* ipaddr,uint16_t port);
struct cf_json* cf_stp_client_request(struct cf_stp_client* client,const char* topic,struct cf_json* msg);
int cf_stp_client_write_binary(struct cf_stp_client* client,const uint8_t* data,size_t data_len);
struct cf_json* cf_stp_client_recv_multicast(struct cf_stp_client* client);
void cf_stp_client_destroy(struct cf_stp_client* client);

#endif//CF_STP_H
