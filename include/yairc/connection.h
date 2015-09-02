#ifndef YI__CONNECTION_H
#define YI__CONNECTION_H

#include "yairc/unet.h"

#include <fcntl.h>
#include <stdlib.h>

typedef struct yi_connection_t {
  struct addrinfo* addrinfo;
  int sockfd;
  // TODO maybe hold here the host and serv as well
  //      would improve debugging
  // char* host   --> dup
  // char* serv   --> dup
} yi_connection_t;

void yi_delete_connection(yi_connection_t* conn);
yi_connection_t* yi_tcp_connect(const char* host, const char* serv);

#endif