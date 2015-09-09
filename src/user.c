#include "yairc/user.h"

yi_user_t* yi_user_create(yi_connection_t* conn)
{
  yi_user_t* user = malloc(sizeof(*user));
  ASSERT(user, "Couldn't allocate memory properly");

  user->conn = conn;
  user->channels_count = 0;
  user->realname[0] = '\0';
  user->nickname[0] = '\0';
  user->username[0] = '\0';

  return user;
}

void yi_user_destroy(yi_user_t* user)
{
  yi_connection_destroy(user->conn);
  FREE(user);
}
