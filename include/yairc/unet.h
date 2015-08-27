#ifndef YI__UNET_H
#define YI__UNET_H

/**
 * Unix Networking
 *
 * Wraps unix networking related functions w/
 * error handling and/or default parameters
 * properly filled.
 */

#include "yairc/debug.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

// SIZES
#define YI_LISTEN_BACKLOG 1024
#define YI_MAXLINE 4096
#define YI_BUFFSIZE 8192

#define YI_INET_ADDR_STRLEN 16
#define YI_INET6_ADDR_STRLEN 46

// PORTS. See more @ /etc/services
#define YI_PORT_DAYTIME 13
#define YI_PORT_ECHO 7
#define YI_PORT_IRC 6667
#define YI_PORT_SMTP 25

typedef struct sockaddr SA;

struct addrinfo *yi_getaddr(const char *host, const char *serv, int family,
                            int socktype);

int yi_socket(int family, int type, int protocol);

int yi_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

int yi_listen(int sockfd, int backlog);

int yi_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

int yi_close(int fd);

int yi_write(int fd, const void *buf, size_t count);

int yi_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

ssize_t write_n(int fd, const void *vptr, size_t n);

ssize_t yi_read_n(int fd, void *vptr, size_t n);

void yi_write_ne(int fd, const void *ptr, size_t nbytes);

void yi_read_ne(int fd, void *ptr, size_t nbytes);

const char *yi_inet_ntop(int family, const void *addrptr, char *strptr,
                         size_t len);

void yi_inet_pton(int family, const char *strptr, void *addrptr);

#endif
