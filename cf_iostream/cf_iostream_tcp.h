#ifndef CF_IOSTREAM_TCP_H
#define CF_IOSTREAM_TCP_H
#include <stdint.h>
#include "cf_iostream/cf_iostream.h"
cf_iostream* cf_iostream_tcp_connect(const char* ipaddr,uint16_t port);
#endif//CF_IOSTREAM_TCP_H