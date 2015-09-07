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
  yi_connection_t* connection = yi_tcp_listen(NULL, "ircd");
  struct sockaddr_in client_addr;
  unsigned client_len;
  int conn_fd;
  int child_pid;

#ifndef NDEBUG
  setbuf(stdout, NULL);
#endif

  // set up the signal handler for SIGCHLD
  yi_signal(SIGCHLD, sig_chld);

  while (1) {
    client_len = sizeof(client_addr);
    conn_fd = yi_accept(connection->sockfd, (SA*)&client_addr, &client_len);
    LOG("%s\n", "connection accepted!\n");

    if (!~conn_fd) {
      if (errno == EINTR)
        continue;
      LOGERR("Server Accept Error.");
      exit(EXIT_FAILURE);
    }

    if ((!(child_pid = fork()))) { // child
      DLOG("Client Connected!\n");
      yi_close(connection->sockfd);
      yi_read_incoming(conn_fd, consume_message);
      exit(EXIT_SUCCESS);
    }

    yi_close(conn_fd); // parent continues
  }

  return 0;
}
