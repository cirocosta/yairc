#include "yairc/connection.h"

yi_connection_t* yi_connection_create()
{
  yi_connection_t* conn = malloc(sizeof(*conn));
  ASSERT(conn, "Couldn't properly allocate memory");

  memset(conn->host, '\0', YI_MAX_NAME);
  conn->addrinfo = NULL;
  conn->sockfd = -1;
  conn->port = 0;

  return conn;
}

void yi_connection_destroy(yi_connection_t* conn)
{
  yi_close(conn->sockfd);
  freeaddrinfo(conn->addrinfo);
  FREE(conn);
}

yi_connection_t* yi_connection_accept(int listen_sock_fd)
{
  struct sockaddr_in client_addr;
  socklen_t len;
  yi_connection_t* conn = yi_connection_create();

  conn->sockfd = yi_accept(listen_sock_fd, (SA*)&client_addr, &len);
  inet_ntop(AF_INET, &client_addr.sin_addr, conn->host, YI_MAX_NAME);
  conn->port = ntohs(client_addr.sin_port);

  return conn;
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

  ASSERT(res, "address %s (serv: %s) - couldn't establish connection", host,
         serv);

  connection = yi_connection_create();
  connection->addrinfo = res;
  connection->sockfd = sockfd;
  memcpy(connection->host, host, strlen(host));
  // TODO get the port

  return connection;
}

yi_connection_t* yi_tcp_listen(const char* host, const char* serv)
{
  yi_connection_t* connection = NULL;
  int listenfd, n;
  const int on = 1;
  struct addrinfo hints, *res;

  bzero(&hints, sizeof(struct addrinfo));
  hints.ai_flags = AI_PASSIVE;
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  ASSERT(!(n = getaddrinfo(host, serv, &hints, &res)),
         "tcp_listen error for %s, %s: %s", host, serv, gai_strerror(n));

  do {
    listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (listenfd < 0)
      continue; /* error, try next one */

    ASSERT(~setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)),
           "couldn't set socket options properly");

    // able to succesfully bind --> OK!
    if (!bind(listenfd, res->ai_addr, res->ai_addrlen))
      break;

    yi_close(listenfd);
  } while ((res = res->ai_next) != NULL);

  ASSERT(res, "tcp_listen error for %s:%s", host, serv);
  yi_listen(listenfd, YI_MAX_BACKLOG);

  connection = yi_connection_create();
  connection->addrinfo = res;
  connection->sockfd = listenfd;
  inet_ntop(AF_INET, res->ai_addr, connection->host, YI_MAX_NAME);

  return connection;
}

void yi_read_incoming(yi_connection_t* conn,
                      void (*process_message)(yi_connection_t* sconn,
                                              yi_message_t* modified_msg))
{
  yi_message_t* message = yi_message_create(NULL, 0);
  char buf[YI_MAX_LINE] = { 0 };
  char out_buf[YI_MAX_MESSAGE] = { 0 };
  char* la = NULL;
  char* tmp = NULL;

  int nread = 0;
  unsigned tot_read = 0;
  unsigned len = 0;

  while (1) {
    if ((nread = read(conn->sockfd, buf + tot_read, YI_MAX_LINE - tot_read)) <
        0) {
      perror("yi_read_incoming::read error:");
      break;
    } else if (!nread) {
      break;
    }

    tot_read += nread;
    tmp = buf;
    la = strstr(tmp, CRLF);

    if (!la)
      continue;

    do {
      la += 2; // points to \n
      len = la - tmp;

      memcpy(out_buf, tmp, len);
      out_buf[len] = '\0';
      tmp = la;
      tot_read -= len;

      if (yi_parse_m(message, out_buf, len) == YI_MESSAGE_NUL)
        continue;

      process_message(conn, message);
    } while ((la = strstr(tmp, CRLF)));

    memmove(buf, tmp, strlen(tmp));
    buf[strlen(tmp)] = '\0';
  }

  yi_message_destroy(message);
}
