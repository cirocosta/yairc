#ifndef YI__SERVER_H
#define YI__SERVER_H

#include "yairc/common.h"
#include "yairc/user.h"
#include "yairc/channel.h"

typedef struct yi_server_t {
  unsigned users_count;
  unsigned channels_count;

  char name[YI_MAX_NAME];
  yi_user_t* users[YI_MAX_USERS];
  yi_channel_t* channels[YI_MAX_USERS_PER_CHAN];
} yi_server_t;

unsigned yi_server_add_user(yi_server_t* server, yi_user_t* user)
{
  server->users[server->users_count] = user;
  user->uid = server->users_count;

  return server->users_count++;
}

void yi_server_remove_user(yi_server_t* server, yi_user_t* user)
{
  unsigned i = 0;

  for (; i < user->channels_count; i++)
    yi_channel_remove_user(server->channels[i], user->uid);
  server->users[user->uid] = NULL;

  yi_user_destroy(user);
}

unsigned yi_server_add_channel(yi_server_t* server, yi_channel_t* channel)
{
  server->channels[server->channels_count] = channel;
  channel->cid = server->channels_count;

  return server->channels_count++;
}

#endif
