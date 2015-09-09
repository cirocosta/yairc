#ifndef YI__SERVER_H
#define YI__SERVER_H

#include "yairc/common.h"
#include "yairc/user.h"
#include "yairc/channel.h"

#include <pthread.h>

typedef struct yi_server_t {
  unsigned users_count;
  unsigned channels_count;

  char name[YI_MAX_NAME];
  yi_user_t* users[YI_MAX_USERS];
  yi_channel_t* channels[YI_MAX_USERS_PER_CHAN];
  pthread_mutex_t mutex;
} yi_server_t;

yi_server_t* yi_server_create();
void yi_server_destroy(yi_server_t* server);

int yi_server_nick_change(yi_server_t* server, yi_user_t* user, char* nick);
int yi_server_user_change(yi_server_t* server, yi_user_t* user, char* username,
                          char* realname);

unsigned yi_server_add_user(yi_server_t* server, yi_user_t* user);
void yi_server_remove_user(yi_server_t* server, yi_user_t* user);
unsigned yi_server_add_channel(yi_server_t* server, yi_channel_t* channel);

#endif
