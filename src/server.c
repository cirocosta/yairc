#include "yairc/server.h"

yi_server_t* yi_server_create(const char* servername, const char* version)
{
  time_t rawtime;
  struct tm* timeinfo;
  yi_server_t* server = malloc(sizeof(*server));
  PASSERT(server, "Couldn't allocate memory properly");

  pthread_mutex_init(&server->mutex, NULL);
  time(&rawtime);
  timeinfo = localtime(&rawtime);

  strftime(server->creation_date, YI_MAX_NAME,
           "This server was created %c at %Z", timeinfo);
  server->conn = yi_tcp_listen(NULL, "ircd");
  server->users_count = 0;
  server->channels_count = 0;
  strncpy(server->name, servername, YI_MAX_NAME);
  strncpy(server->version, version, YI_MAX_NAME);

  // TODO verify what these characters actually mean
  strncpy(server->user_modes, "aiwroOs", YI_MAX_NAME);
  strncpy(server->channel_modes, "oOvaimnpqrstklbeI", YI_MAX_NAME);

  return server;
}

void yi_server_destroy(yi_server_t* server)
{
  yi_connection_destroy(server->conn);
  pthread_mutex_destroy(&server->mutex);
}

int yi_server_send_message(yi_server_t* server, yi_user_t* user, char* command,
                           char* params)
{
  char msg[YI_MAX_MESSAGE] = { 0 };

  snprintf(msg, YI_MAX_MESSAGE, ":%s %s %s\r\n", server->conn->host, command,
           params);
  yi_write_ne(user->conn->sockfd, msg);

  return 1;
}

int yi_server_welcome_user(yi_server_t* server, yi_user_t* user)
{
  char buf1[YI_MAX_MESSAGE] = { 0 };
  char buf2[YI_MAX_MESSAGE] = { 0 };
  char buf3[YI_MAX_MESSAGE] = { 0 };
  char buf4[YI_MAX_MESSAGE] = { 0 };

  snprintf(buf1, YI_MAX_MESSAGE,
           "%s :Welcome to the Internet Relay Network %s!%s@%s", user->nickname,
           user->nickname, user->username, user->conn->host);
  snprintf(buf2, YI_MAX_MESSAGE, "%s :Your host is %s, running version %s",
           user->nickname, server->conn->host, server->version);
  snprintf(buf3, YI_MAX_MESSAGE, "%s :This server was created %s",
           user->nickname, server->creation_date);
  snprintf(buf4, YI_MAX_MESSAGE, "%s %s %s %s %s", user->nickname, server->name,
           server->version, server->user_modes, server->channel_modes);

  yi_server_send_message(server, user, "001", buf1);
  yi_server_send_message(server, user, "002", buf2);
  yi_server_send_message(server, user, "003", buf3);
  yi_server_send_message(server, user, "004", buf4);

  return 1;
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
