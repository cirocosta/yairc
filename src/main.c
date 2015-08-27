#include <stdio.h>
#include "yairc/connection.h"

static const char* CLI_HELP =
    "Usage: yairc <url> <service>\n"
    "\n"
    "url:\tserver authority (url w/out scheme and path\n"
    "serv:\tservice to map port to.\n"
    "\n"
    "Example:\n"
    "\t./yairc irc.mozilla.org dirc\n";

void communicate(FILE* fp, int sockfd)
{
  char sendline[YI_MAXLINE] = { 0 };
  char recvline[YI_MAXLINE] = { 0 };

  if (fgets(sendline, YI_MAXLINE, fp) == NULL || ferror(fp))
    exit(EXIT_FAILURE);

  yi_write_ne(sockfd, sendline, strlen(sendline));
  yi_read_n(sockfd, recvline, strlen(sendline));
  fprintf(stdout, "%s", recvline);
}

int main(int argc, char* argv[])
{
  yi_connection_t* connection;

  if (argc != 3) {
    fprintf(stderr, "%s\n", CLI_HELP);
    return EXIT_FAILURE;
  }

  connection = yi_tcp_connect(argv[1], argv[2]);
  communicate(stdin, connection->sockfd);

  yi_close(connection->sockfd);
  yi_delete_connection(connection);

  return EXIT_SUCCESS;
}
