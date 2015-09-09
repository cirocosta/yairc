#include "yairc/commands.h"
#include "yairc/common.h"
#include "yairc/server.h"
#include "yairc/parser.h"

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

yi_server_t* g_server;

/**
 * processes a message that came from a user
 */
void process_message(void* arg, yi_message_t* message)
{
  ENTRY e, *ep;
  yi_user_t* user = (yi_user_t*)arg;

  LOG("%d\tusername = `%s`", user->uid, user->username);
  LOG("%d\tcommand = `%s`", user->uid, message->command);
  LOG("%d\tprefix = `%s`", user->uid, message->prefix);
  LOG("%d\tparameters[%d] = `%s`", user->uid, 0, message->parameters[0]);
  LOG("--");

  if (message->command[0] == '\0')
    return;

  e.key = message->command;
  ep = hsearch(e, FIND);

  if (!ep) {
    LOGERR("\tuid[%u]: command %s not found.", user->uid, message->command);
    return;
  }

  (*(yi_command_callback)ep->data)(g_server, user, message);
}

/**
 * A thread-simulated user process within the
 * server
 */
void* user_process(void* arg)
{
  yi_user_t* user = (yi_user_t*)arg;

  DLOG("\tFrom inside the user process");
  yi_message_parse_fd(user->conn->sockfd, (void*)user, process_message);
  DLOG("\tConnection end. Removing user.");

  yi_server_remove_user(g_server, user);
  yi_user_destroy(user);

  pthread_exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[])
{
  unsigned i = 0;
  pthread_t tids[YI_MAX_USERS] = { 0 };
  yi_connection_t* client_connection = NULL;
  yi_user_t* client = NULL;
  g_server = yi_server_create("YIServer", "YI0.0.1");

#ifndef NDEBUG
  setbuf(stdout, NULL);
#endif
  yi_commands_table_init();

  while (1) {
    DLOG("waiting for connection...");
    client_connection = yi_connection_accept(g_server->conn->sockfd);
    DLOG("Client %s Connected!", client_connection->host);
    client = yi_user_create(client_connection);
    yi_server_add_user(g_server, client);
    pthread_create(&tids[i++], NULL, user_process, (void*)client);
  }

  yi_server_destroy(g_server);
  yi_commands_table_destroy();
  pthread_exit(EXIT_SUCCESS);
}
