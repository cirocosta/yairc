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

int yi_command_QUIT(yi_server_t* server, yi_user_t* user, yi_message_t* message)
{
  yi_server_send_message(server, user, "ERROR", NULL);

  return 1;
}

int yi_command_LIST(yi_server_t* server, yi_user_t* user, yi_message_t* message)
{
  unsigned i = 0;
  char buf1[YI_MAX_MESSAGE] = {0};
  char buf2[YI_MAX_MESSAGE] = {0};
  char buf3[YI_MAX_MESSAGE] = {0};

  snprintf(buf1, YI_MAX_MESSAGE, "%s Channel :Users Name", user->nickname);
  yi_server_send_message(server, user, "321", buf1);

  for (; i < YI_MAX_CHANNELS; i++) {
    if (!server->channels[i])
      continue;

    snprintf(buf2, YI_MAX_MESSAGE, "%s %s %d :%s", user->nickname,
             server->channels[i]->name, server->channels[i]->users_count,
             server->channels[i]->topic);
    yi_server_send_message(server, user, "322", buf2);
  }

  snprintf(buf3, YI_MAX_MESSAGE, "%s :End of channel list.", user->nickname);
  yi_server_send_message(server, user, "323", buf3);

  return 0;
}

int yi_command_PING(yi_server_t* server, yi_user_t* user, yi_message_t* message)
{
  char buf[YI_MAX_MESSAGE] = { 0 };

  if (message->parameters_count < 1) {
    LOGERR("Malformed PING command.");
    return 0;
  }

  snprintf(buf, YI_MAX_MESSAGE, "%s :%s", server->conn->host,
           message->parameters[0]);

  yi_server_send_message(server, user, "PONG", buf);

  return 1;
}
int yi_command_PONG(yi_server_t* server, yi_user_t* user, yi_message_t* message)
{
  if (!strcmp(message->parameters[0], server->conn->host)) {
    user->pinged = 0;
    return 1;
  }

  LOGERR("Malformed PONG response");

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
  char buf1[YI_MAX_MESSAGE] = { 0 };
  char buf2[YI_MAX_MESSAGE] = { 0 };
  char buf3[YI_MAX_MESSAGE] = { 0 };
  char buf4[YI_MAX_MESSAGE] = { 0 };

  if (message->parameters_count < 4) {
    LOGERR("USER: not enough parameters");
    return 0;
  }

  if (!yi_server_user_change(server, user, message->parameters[0],
                            message->parameters[3])) {
    // TODO send the proper error message back to the user 
    LOGERR("USER: user already exists!");
    return 0;
  }

  snprintf(buf1, YI_MAX_MESSAGE,
           "%s :Welcome to the Internet Relay Network %s!%s@%s", user->nickname,
           user->nickname, user->username, user->conn->host);
  snprintf(buf2, YI_MAX_MESSAGE, "%s :Your host is %s, running version %s",
           user->nickname, server->conn->host, server->version);
  snprintf(buf3, YI_MAX_MESSAGE, "%s :%s", user->nickname,
           server->creation_date);
  snprintf(buf4, YI_MAX_MESSAGE, "%s %s %s %s %s", user->nickname, server->name,
           server->version, server->user_modes, server->channel_modes);

  yi_server_send_message(server, user, "001", buf1);
  yi_server_send_message(server, user, "002", buf2);
  yi_server_send_message(server, user, "003", buf3);
  yi_server_send_message(server, user, "004", buf4);

  return 1;
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
