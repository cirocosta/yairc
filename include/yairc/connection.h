#ifndef YI__CONNECTION_H
#define YI__CONNECTION_H

#include "yairc/unet.h"
#include "yairc/parser.h"

#include <linux/limits.h>
#include <fcntl.h>
#include <stdlib.h>

typedef struct yi_connection_t {
  struct addrinfo* addrinfo;
  int sockfd;
  char host[NAME_MAX];
  uint16_t port;
} yi_connection_t;

yi_connection_t* yi_connection_create();
void yi_connection_destroy(yi_connection_t* conn);
yi_connection_t* yi_connection_accept(int listen_sock_fd);

yi_connection_t* yi_tcp_connect(const char* host, const char* serv);
yi_connection_t* yi_tcp_listen(const char* host, const char* service);

void yi_read_incoming(yi_connection_t* connection,
                      void (*process_message)(yi_connection_t* conn,
                                              yi_message_t* msg));

#endif
