#include "yairc/commands.h"

void yi_commands_table_init()
{
  ENTRY e, *ep;
  int i = 0;

  hcreate(YI_COMMANDS_SIZE);

  for (; i < YI_COMMANDS_SIZE; i++) {
    e.key = YI_COMMANDS[i].key;
    e.data = YI_COMMANDS[i].callback;
    ep = hsearch(e, ENTER);
    ASSERT(ep, "Couldn't insert command to HTABLE");
  }
}

void yi_commands_table_destroy()
{
  hdestroy();
}

// FIXME should close the connection properly (send ERROR)
int yi_command_QUIT(yi_server_t* server, yi_user_t* user, yi_message_t* message)
{
  return 0;
}

  /* 001    RPL_WELCOME */
  /*     "Welcome to the Internet Relay Network */
  /*      <nick>!<user>@<host>" */
  /* 002    RPL_YOURHOST */
  /*     "Your host is <servername>, running version <ver>" */
  /* 003    RPL_CREATED */
  /*     "This server was created <date>" */
  /* 004    RPL_MYINFO */
  /*     "<servername> <version> <available user modes> */
  /*      <available channel modes>" */ 
int yi_command_USER(yi_server_t* server, yi_user_t* user, yi_message_t* message)
{
  if (message->parameters_count < 4) {
    LOGERR("USER: not enough parameters");
    return 0;
  }

  if (yi_server_user_change(server, user, message->parameters[0],
                            message->parameters[3])) {
    yi_server_welcome_user(server, user);
    return 1;
  }

  // TODO say that the user exists, whatever

  return 0;
}

int yi_command_NICK(yi_server_t* server, yi_user_t* user, yi_message_t* message)
{
  if (message->parameters_count < 1) {
    LOGERR("NICK: not enough parameters");

    return 0;
  }

  if (yi_server_nick_change(server, user, message->parameters[0]))
    return 1;

  return 0;
}
