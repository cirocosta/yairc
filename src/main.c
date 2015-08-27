#include <stdio.h>
#include "yairc/connection.h"

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

  ASSERT(argc == 2, "Usage: yairc <url>");

  connection = yi_tcp_connect(argv[1], "echo");
  communicate(stdin, connection->sockfd);

  yi_close(connection->sockfd);
  yi_delete_connection(connection);

  LOG("heu");

  return 0;
}
