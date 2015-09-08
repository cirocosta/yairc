#ifndef YI__CHANNEL_H
#define YI__CHANNEL_H

#include "yairc/common.h"
#include "yairc/user.h"

typedef struct yi_channel_t {
  char name[YI_MAX_NAME];
  char topic[YI_MAX_NAME];
  unsigned cid;
  yi_user_t* users[YI_MAX_USERS];
  unsigned users_count;
} yi_channel_t;

yi_channel_t* yi_channel_create()
{
  yi_channel_t* channel = malloc(sizeof(*channel));

  channel->cid = 0;
  channel->users_count = 0;
  memset(channel->name, '\0', YI_MAX_NAME);
  memset(channel->topic, '\0', YI_MAX_NAME);

  return channel;
}

void yi_channel_destroy(yi_channel_t* channel)
{
  FREE(channel);
}

void yi_channel_remove_user(yi_channel_t* channel, unsigned uid)
{
}

void yi_channel_add_user(yi_channel_t* channel, yi_user_t* user)
{
}

void yi_channel_broadcast_message(yi_channel_t* channel, const char* message)
{
}

#endif
