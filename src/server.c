#include "yairc/server.h"

yi_server_t* yi_server_create()
{
  yi_server_t* server = malloc(sizeof(*server));
  PASSERT(server, "Couldn't allocate memory properly");

  pthread_mutex_init(&server->mutex, NULL);

  server->users_count = 0;
  server->channels_count = 0;
  memset(server->name, '\0', YI_MAX_NAME);

  return server;
}

void yi_server_destroy(yi_server_t* server)
{
  pthread_mutex_destroy(&server->mutex);
}

int yi_server_user_change(yi_server_t* server, yi_user_t* user, char* username,
                          char* realname)
{
  unsigned i = 0;

  for (; i < server->users_count; i++)
    if (!strcmp(server->users[i]->username, username))
      return 0;

  strncpy(server->users[user->uid]->username, username, strlen(username));
  strncpy(server->users[user->uid]->realname, realname, strlen(realname));

  return 1;
}

int yi_server_nick_change(yi_server_t* server, yi_user_t* user, char* nick)
{
  unsigned i = 0;

  for (; i < server->users_count; i++)
    if (!strcmp(server->users[i]->username, nick))
      return 0;

  strncpy(server->users[user->uid]->nickname, nick, strlen(nick));

  return 1;
}

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
}

unsigned yi_server_add_channel(yi_server_t* server, yi_channel_t* channel)
{
  server->channels[server->channels_count] = channel;
  channel->cid = server->channels_count;

  return server->channels_count++;
}
