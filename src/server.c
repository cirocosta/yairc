#include "yairc/server.h"

yi_server_t* yi_server_create(const char* servername, const char* version)
{
  time_t rawtime;
  struct tm* timeinfo;
  yi_server_t* server = malloc(sizeof(*server));
  PASSERT(server, "Couldn't allocate memory properly");

  time(&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(server->creation_date, YI_MAX_NAME,
           "This server was created %c at %Z", timeinfo);

  pthread_mutex_init(&server->mutex, NULL);
  server->conn = yi_tcp_listen(NULL, "ircd");
  server->users_count = 0;
  server->channels_count = 0;
  server->channels = yi_table_create(YI_MAX_CHANNELS);

  strncpy(server->name, servername, YI_MAX_NAME);
  strncpy(server->version, version, YI_MAX_NAME);

  // TODO verify what these characters actually mean
  strncpy(server->user_modes, "aiwroOs", YI_MAX_NAME);
  strncpy(server->channel_modes, "oOvaimnpqrstklbeI", YI_MAX_NAME);

  yi_server_new_channel(server, "#support", "General Support For Newcomers");
  yi_server_new_channel(server, "#development", "Development Discussions");

  return server;
}

void yi_server_destroy(yi_server_t* server)
{
  unsigned i = 0;
  yi_connection_destroy(server->conn);
  pthread_mutex_destroy(&server->mutex);

  for (; i < server->channels->size; i++)
    if (server->channels->data[i])
      yi_channel_destroy((yi_channel_t*)server->channels->data[i]);
  yi_table_destroy(server->channels);
}

int yi_server_ping_user(yi_server_t* server, yi_user_t* user)
{
  yi_server_send_message(server, user, "PING", server->conn->host);
  user->pinged = 1;

  return 0;
}

int yi_server_send_message(yi_server_t* server, yi_user_t* user, char* command,
                           char* params)
{
  char msg[YI_MAX_MESSAGE] = { 0 };

  if (params) {
    snprintf(msg, YI_MAX_MESSAGE, ":%s %s %s\r\n", server->conn->host, command,
             params);
  } else {
    snprintf(msg, YI_MAX_MESSAGE, ":%s %s\r\n", server->conn->host, command);
  }

  yi_write_ne(user->conn->sockfd, msg);

  return 1;
}

int yi_server_user_change(yi_server_t* server, yi_user_t* user, char* username,
                          char* realname)
{
  unsigned i = 0;

  for (; i < server->users_count; i++)
    if (server->users[i] && !strcmp(server->users[i]->username, username))
      return 0;

  strncpy(server->users[user->uid]->username, username, strlen(username));
  strncpy(server->users[user->uid]->realname, realname, strlen(realname));

  return 1;
}

int yi_server_nick_change(yi_server_t* server, yi_user_t* user, char* nick)
{
  unsigned i = 0;

  for (; i < server->users_count; i++)
    if (server->users[i] && !strcmp(server->users[i]->username, nick))
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

  for (; i < user->channels_count; i++) {
    if (server->channels_list[user->cids[i]])
      yi_channel_remove_user(server->channels_list[user->cids[i]], user);
  }

  server->users[user->uid] = NULL;
}

yi_channel_t* yi_server_new_channel(yi_server_t* server, char* name,
                                    char* topic)
{
  yi_channel_t* channel = yi_channel_create(name, topic);

  server->channels_list[server->channels_count] = channel;
  yi_table_add(server->channels, channel->name, (void*)channel);
  channel->cid = server->channels_count++;

  return channel;
}

yi_channel_t* yi_server_get_channel(yi_server_t* server, char* name)
{
  return yi_table_get(server->channels, name);
}


