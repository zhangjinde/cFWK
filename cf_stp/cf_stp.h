#ifndef CF_STP_H
#define CF_STP_H

#include "cf_json/cf_json.h"

#define CF_STP_SERVER_RECV_BUFF_SIZE    256
#define CF_STP_CLIENT_RECV_BUFF_SIZE    256

struct cf_stp_server;
struct cf_stp_context;
void cf_stp_server_listen(struct cf_stp_server* server, char* topic,struct cf_json* (*proccessor)(struct cf_stp_context* ,struct cf_json*));

#endif//CF_STP_H
