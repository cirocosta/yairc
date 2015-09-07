#include "yairc/signals.h"
#include "yairc/common.h"
#include "yairc/connection.h"

#include <sys/wait.h>
#include <sys/types.h>

#include <stdlib.h>
#include <stdio.h>

// SIGCHLD is sent by the kernel to the parent whenever a child process
// terminates.
void sig_chld(int signo)
{
  pid_t pid;
  int stat;

  // we can't use simply `wait` here as signals won't be queued by linux.
  while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
    write(STDERR_FILENO, "Child terminated\n", 18);
}

void consume_message(yi_message_t* message)
{
  LOG("command = %s", message->command);
  LOG("prefix = %s\n", message->prefix);
  LOG("parameters[%d] = %s", 0,  message->parameters[0]);
}

int main(int argc, char* argv[])
{
  int listen_fd, conn_fd;
  pid_t child_pid;
  socklen_t client_len;
  struct sockaddr_in client_addr, server_addr;

#ifndef NDEBUG
  setbuf(stdout, NULL);
#endif

  // set up the signal handler for SIGCHLD
  yi_signal(SIGCHLD, sig_chld);
  listen_fd = yi_socket(AF_INET, SOCK_STREAM, 0);
  LOG("%s\n", "socket created");

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(YI_PORT_IRC);
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  yi_bind(listen_fd, (SA*)&server_addr, sizeof(server_addr));
  yi_listen(listen_fd, YI_LISTEN_BACKLOG);
  LOG("%s\n", "listening");

  while (1) {
    client_len = sizeof(client_addr);
    conn_fd = yi_accept(listen_fd, (SA*)&client_addr, &client_len);
    LOG("%s\n", "connection accepted!\n");

    if (!~conn_fd) {
      if (errno == EINTR)
        continue;
      LOGERR("Server Accept Error.");
      exit(EXIT_FAILURE);
    }

    if ((!(child_pid = fork()))) { // child
      DLOG("Client Connected!\n");
      yi_close(listen_fd);
      yi_read_incoming(conn_fd, consume_message);
      exit(EXIT_SUCCESS);
    }

    yi_close(conn_fd); // parent continues
  }

  return 0;
}
