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

int yi_command_USER(yi_server_t* server, yi_user_t* user, yi_message_t* message)
{
  if (message->parameters_count < 4) {
    LOGERR("USER: not enough parameters");
    return 0;
  }

  return yi_server_user_change(server, user, message->parameters[0], message->parameters[3]);
}

int yi_command_NICK(yi_server_t* server, yi_user_t* user, yi_message_t* message)
{
  if (message->parameters_count < 1) {
    LOGERR("NICK: not enough parameters");
    return 0;
  }
  
  return yi_server_nick_change(server, user, message->parameters[0]);
}


