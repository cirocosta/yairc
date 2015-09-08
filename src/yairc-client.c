#include "yairc/connection.h"
#include "yairc/parser.h"

#include <stdio.h>
#include <string.h>

#define WRITE(__fd, __str) write(__fd, __str, strlen(__str))

static const char* CLI_HELP =
    "Usage: yairc <url> <service>\n"
    "\n"
    "url:\tserver authority (url w/out scheme and path\n"
    "serv:\tservice to map port to.\n"
    "\n"
    "Example:\n"
    "\t./yairc irc.mozilla.org ircd\n";

void read_messages(int sockfd)
{
  char buf[YI_MAXLINE] = { 0 };
  char out_buf[YI_MAX_MESSAGE_SIZE] = { 0 };
  char* la = NULL;
  char* tmp = NULL;

  int nread = 0;
  unsigned tot_read = 0;
  unsigned len = 0;

  while (1) {
    if ((nread = read(sockfd, buf + tot_read, YI_MAXLINE - tot_read)) < 0) {
      perror("read error:");
      return;
    } else if (!nread) {
      memset(buf, '\0', tot_read + 1);
      tot_read = 0;
      sleep(1);
      continue;
    }

    tot_read += nread;
    tmp = buf;
    la = strstr(tmp, CRLF);

    if (!la)
      continue;

    do {
      la += 1; // points to \n
      len = la - tmp;

      memcpy(out_buf, tmp, len);
      out_buf[len] = '\0';
      tmp = la;

      tot_read -= len;
      write(STDOUT_FILENO, out_buf, len);
    } while ((la = strstr(tmp, CRLF)));

    memmove(buf, tmp, strlen(tmp));
    buf[strlen(tmp)] = '\0';
  }
}

int main(int argc, char* argv[])
{
  yi_connection_t* connection;

#ifndef NDEBUG
  setbuf(stdout, NULL);
#endif

  if (argc != 3) {
    fprintf(stderr, "%s\n", CLI_HELP);
    return EXIT_FAILURE;
  }

  connection = yi_tcp_connect(argv[1], argv[2]);
  LOG("Connection Established!\n");

  WRITE(connection->sockfd, "NICK guest812938\r\n");
  WRITE(connection->sockfd, "USER guest812938 0 * :A Cool Guest\r\n");

  read_messages(connection->sockfd);

  yi_connection_destroy(connection);

  return EXIT_SUCCESS;
}
