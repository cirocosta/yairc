#include <stdio.h>
#include "yairc/unet.h"

void communicate(FILE* fp, int sockfd)
{
  char sendline[YI_MAXLINE] = { 0 };
  char recvline[YI_MAXLINE] = { 0 };

  if (fgets(sendline, YI_MAXLINE, fp) == NULL || ferror(fp))
    exit(EXIT_FAILURE);

  yi_write_ne(sockfd, sendline, strlen(sendline));
  yi_read_n(sockfd, recvline, YI_MAXLINE);
  fputs(recvline, stdout);
}

int main(int argc, char* argv[])
{
  int sock_fd;
  struct sockaddr_in server_addr;

  sock_fd = yi_socket(AF_INET, SOCK_STREAM, 0);
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(YI_PORT_ECHO);
  yi_inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

  yi_connect(sock_fd, (SA*)&server_addr, sizeof(server_addr));
  communicate(stdin, sock_fd);
  yi_close(sock_fd);

  return 0;
}
