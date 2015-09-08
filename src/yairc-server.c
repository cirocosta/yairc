#include "yairc/common.h"
#include "yairc/connection.h"
#include "yairc/user.h"
#include "yairc/server.h"

#include <sys/wait.h>
#include <sys/types.h>

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * Client connects:
 *  - A new thread is spawn for the user.
 *    The thread receives a reference to the user object in the
 *    server's 'database', then it can read from the user's socket,
 *    blocking on read.
 */

yi_server_t g_server;

void process_message(yi_connection_t* conn, yi_message_t* message)
{
  LOG("command = %s", message->command);
  LOG("prefix = %s\n", message->prefix);
  LOG("parameters[%d] = %s", 0, message->parameters[0]);
}

void* user_process(void* arg)
{
  yi_user_t* user = (yi_user_t*)arg;

  DLOG("\tFrom inside the user process");

  yi_read_incoming(user->conn, process_message);

  yi_server_remove_user(&g_server, user);
  pthread_exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[])
{
  unsigned i = 0;
  pthread_t tids[YI_MAX_USERS] = { 0 };
  const yi_connection_t* listen_connection = yi_tcp_listen(NULL, "ircd");
  yi_connection_t* client_connection = NULL;
  yi_user_t* client = NULL;

#ifndef NDEBUG
  setbuf(stdout, NULL);
#endif

  while (1) {
    DLOG("waiting for connection...");
    client_connection = yi_connection_accept(listen_connection->sockfd);
    DLOG("Client Connected!");
    client = yi_user_create(client_connection);
    yi_server_add_user(&g_server, client);
    pthread_create(&tids[i++], NULL, user_process, (void*)client);
  }

  pthread_exit(EXIT_SUCCESS);
}
