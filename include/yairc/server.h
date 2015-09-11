#ifndef YI__SERVER_H
#define YI__SERVER_H

#include "yairc/common.h"
#include "yairc/user.h"
#include "yairc/channel.h"
#include "yairc/table.h"
#include "yairc/connection.h"

#include <time.h>
#include <pthread.h>

typedef struct yi_server_t {
  // general
  char name[YI_MAX_NAME];
  pthread_mutex_t mutex;
  yi_connection_t* conn;
  char creation_date[YI_MAX_NAME];
  char version[YI_MAX_NAME];

  // users
  unsigned users_count;
  yi_user_t* users[YI_MAX_USERS];
  char user_modes[YI_MAX_NAME];

  // channels
  unsigned channels_count;
  char channel_modes[YI_MAX_NAME];
  yi_table_t* channels; //  YI_MAX_CHANNELS
  yi_channel_t* channels_list[YI_MAX_CHANNELS];
} yi_server_t;

// ctor dtor
yi_server_t* yi_server_create();
void yi_server_destroy(yi_server_t* server);

// commands related
int yi_server_nick_change(yi_server_t* server, yi_user_t* user, char* nick);
int yi_server_user_change(yi_server_t* server, yi_user_t* user, char* username,
                          char* realname);
int yi_server_ping_user(yi_server_t*, yi_user_t*);

// general
unsigned yi_server_add_user(yi_server_t* server, yi_user_t* user);
void yi_server_remove_user(yi_server_t* server, yi_user_t* user);

yi_channel_t* yi_server_new_channel(yi_server_t* server, char* name,
                                    char* topic);
yi_channel_t* yi_server_get_channel(yi_server_t* server, char* name);

// TODO improve this with buffering capabilities (reduce 'write' calls)
int yi_server_send_message(yi_server_t* server, yi_user_t* user, char* command,
                           char* params);

#endif
