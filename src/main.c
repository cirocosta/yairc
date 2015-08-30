#include <stdio.h>
#include "yairc/connection.h"

#define WRITE(__fd, __str) write(__fd, __str, strlen(__str))

static const char* CLI_HELP =
    "Usage: yairc <url> <service>\n"
    "\n"
    "url:\tserver authority (url w/out scheme and path\n"
    "serv:\tservice to map port to.\n"
    "\n"
    "Example:\n"
    "\t./yairc irc.mozilla.org ircd\n";

void communicate(int sockfd)
{
  size_t nread = 0;
  char recvline[YI_MAXLINE] = { 0 };

  WRITE(sockfd, "NICK guest\r\n");
  WRITE(sockfd, "USER guest 0 * :A Cool Guest\r\n");

  while ((nread = read(sockfd, recvline, YI_MAXLINE))) {
    write(STDOUT_FILENO, recvline, nread + 1);
  }

  memset(recvline, '\0', nread + 1);
}

int main(int argc, char* argv[])
{
  yi_connection_t* connection;

  if (argc != 3) {
    fprintf(stderr, "%s\n", CLI_HELP);
    return EXIT_FAILURE;
  }

  connection = yi_tcp_connect(argv[1], argv[2]);
  LOG("Connection Established!");
  communicate(connection->sockfd);

  yi_close(connection->sockfd);
  yi_delete_connection(connection);

  return EXIT_SUCCESS;
}
