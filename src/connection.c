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

  ASSERT(res, "address %s (serv: %s) - couldn't establish connection", host,
         serv);

  connection = malloc(sizeof(*connection));
  connection->addrinfo = res;
  connection->sockfd = sockfd;

  return connection;
}

void yi_read_incoming(int fd,
                      void (*process_message)(yi_message_t* modified_msg))
{
  yi_message_t* message = yi_message_create(NULL, 0);
  char buf[YI_MAXLINE] = { 0 };
  char out_buf[513] = { 0 };
  char* la = NULL;
  char* tmp = NULL;

  int nread = 0;
  unsigned tot_read = 0;
  unsigned len = 0;

  while (1) {
    if ((nread = read(fd, buf + tot_read, YI_MAXLINE - tot_read)) < 0) {
      perror("yi_read_incoming::read error:");
      break;
    } else if (!nread) {
      memset(buf, '\0', tot_read + 1);
      tot_read = 0;
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

      yi_parse_m(message, out_buf, len);
      process_message(message);
    } while ((la = strstr(tmp, CRLF)));

    memmove(buf, tmp, strlen(tmp));
    buf[strlen(tmp)] = '\0';
  }

  yi_message_destroy(message);
}
