#include "yairc/connection.h"

void yi_delete_connection(yi_connection_t* conn)
{
  close(conn->sockfd);
  freeaddrinfo(conn->addrinfo);
  FREE(conn);
}

yi_connection_t* yi_tcp_connect(const char* host, const char* serv)
{
  struct addrinfo* res = yi_getaddr(host, serv, AF_UNSPEC, SOCK_STREAM);
  yi_connection_t* connection = NULL;
  int sockfd;

  if (!res) {
    LOGERR("address %s (serv: %s) not found.", host, serv);
    return NULL;
  }

  do {
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    if (!~sockfd)
      continue;
    if (!connect(sockfd, res->ai_addr, res->ai_addrlen))
      break;
    yi_close(sockfd);
  } while ((res = res->ai_next));

  if (!res) {
    LOGERR("address %s (serv: %s) - couldn't establish connection", host, serv);
    return NULL;
  }

  connection = malloc(sizeof(*connection));
  connection->addrinfo = res;
  connection->sockfd = sockfd;

  return connection;
}

