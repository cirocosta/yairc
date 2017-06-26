#ifndef YI__CONNECTION_H
#define YI__CONNECTION_H

#include "yairc/common.h"
#include "yairc/unet.h"

#include <fcntl.h>
#include <stdlib.h>

typedef struct yi_connection_t {
  struct addrinfo* addrinfo;
  int sockfd;
  char host[YI_MAX_NAME];
  uint16_t port;
} yi_connection_t;

yi_connection_t* yi_connection_create();
void yi_connection_destroy(yi_connection_t* conn);
yi_connection_t* yi_connection_accept(int listen_sock_fd);

yi_connection_t* yi_tcp_connect(const char* host, const char* serv);
yi_connection_t* yi_tcp_listen(const char* host, const char* service);

#endif
