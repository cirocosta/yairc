#include "yairc/channel.h"

yi_channel_t* yi_channel_create(char* name, char* topic)
{
  yi_channel_t* channel = malloc(sizeof(*channel));

  channel->cid = 0;
  channel->users_count = 0;

  strncpy(channel->name, name, YI_MAX_NAME);
  strncpy(channel->topic, topic, YI_MAX_NAME);

  return channel;
}

void yi_channel_destroy(yi_channel_t* channel)
{
  FREE(channel);
}

void yi_channel_remove_user(yi_channel_t* channel, yi_user_t* user)
{
  LOGERR("CHAN REMOVE USER - NOT IMPLEMENTED");
}

void yi_channel_add_user(yi_channel_t* channel, yi_user_t* user)
{
  channel->users[channel->users_count] = user;
  channel->users_count++;
}

void yi_channel_broadcast_message(yi_channel_t* channel, const char* message)
{
  LOGERR("NOT IMPLEMENTED");
}
