#include "yairc/unet.h"

struct addrinfo *yi_getaddr(const char *host, const char *serv, int family,
                            int socktype)
{
  int n;
  struct addrinfo hints;
  struct addrinfo *res;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_flags = AI_CANONNAME;
  hints.ai_family = family;
  hints.ai_socktype = socktype;

  if ((n = getaddrinfo(host, serv, &hints, &res))) {
    DLOGERR("yi_getaddr error for %s, %s: %s",
            (host == NULL) ? "(no hostname)" : host,
            (serv == NULL) ? "(no service name)" : serv, gai_strerror(n));
    return NULL;
  }

  return res;
}

int yi_socket(int family, int type, int protocol)
{
  int n;
  ASSERT(~(n = socket(family, type, protocol)), "socket error: %s",
         strerror(errno));
  return n;
}

int yi_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
  int n;
  ASSERT(~(n = bind(sockfd, addr, addrlen)), "bind error: %s", strerror(errno));
  return n;
}

int yi_listen(int sockfd, int backlog)
{
  int n;
  ASSERT(~(n = listen(sockfd, backlog)), "bind error: %s", strerror(errno));
  return n;
}

int yi_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
  int n;

  *addrlen = sizeof(*addr);
  // we could have an ECONNABORTED here, in which case we'd not
  // really exit w/ a failure, but ignore.
  ASSERT(~(n = accept(sockfd, addr, addrlen)), "accept error: %s",
         strerror(errno));

  return n;
}

int yi_close(int fd)
{
  int n;
  ASSERT(~(n = close(fd)), "close error: %s", strerror(errno));
  return n;
}

int yi_write(int fd, const void *buf, size_t count)
{
  int n;
  ASSERT(~(n = write(fd, buf, count)), "write error: %s", strerror(errno));
  return n;
}

int yi_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
  int n;
  ASSERT(~(n = connect(sockfd, addr, addrlen)), "connect error: %s",
         strerror(errno));
  return n;
}

ssize_t write_n(int fd, const void *vptr, size_t n)
{
  size_t nleft = n;
  const char *ptr = vptr;
  ssize_t nwritten;

  while (nleft > 0) {
    if ((nwritten = write(fd, ptr, nleft)) <= 0) {
      if (nwritten < 0 && errno == EINTR)
        nwritten = 0;
      else
        return -1;
    }

    nleft -= nwritten;
    ptr += nwritten;
  }

  return nwritten;
}

void yi_write_ne(int fd, const void *ptr, size_t nbytes)
{
  ASSERT(write_n(fd, ptr, nbytes) == nbytes,
         "Error during write_n: not enought bytes written");
}

const char *yi_inet_ntop(int family, const void *addrptr, char *strptr,
                         size_t len)
{
  const char *ptr;

  ASSERT(strptr, "3rd arg to inet_ntop must not be NULL.");
  ptr = inet_ntop(family, addrptr, strptr, len);
  PASSERT(ptr, "inet_ntop error");

  return (ptr);
}

void yi_inet_pton(int family, const char *strptr, void *addrptr)
{
  int n;

  PASSERT((n = inet_pton(family, strptr, addrptr) >= 0),
          "inet_pton error for %s", strptr); /* errno set */
  if (n == 0) {
    LOGERR("inet_pton error for %s", strptr); /* errno not set */
    exit(EXIT_FAILURE);
  }
}
