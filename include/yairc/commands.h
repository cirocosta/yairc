#ifndef YI__COMMANDS_H
#define YI__COMMANDS_H

#include "yairc/server.h"
#include "yairc/parser.h"

#include <string.h>
#include <stdio.h>
#include <search.h>

typedef int (*yi_command_callback)(yi_server_t* server, yi_user_t* user,
                                   yi_message_t* message);

typedef struct yi_command_t {
  char* key;
  yi_command_callback callback;
} yi_command_t;

int yi_command_NICK(yi_server_t* server, yi_user_t* user,
                    yi_message_t* message);
int yi_command_USER(yi_server_t* server, yi_user_t* user,
                    yi_message_t* message);

const static yi_command_t YI_COMMANDS[] = {
  {.key = "USER", .callback = yi_command_USER },
  {.key = "NICK", .callback = yi_command_NICK },
  {.key = "PING", .callback = NULL },
  {.key = "PONG", .callback = NULL },
};

#define YI_COMMANDS_SIZE (sizeof(YI_COMMANDS) / sizeof(YI_COMMANDS[0]))

void yi_commands_table_init();
void yi_commands_table_destroy();

#endif
