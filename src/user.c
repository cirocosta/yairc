#include "yairc/user.h"

yi_user_t* yi_user_create(yi_connection_t* conn)
{
  yi_user_t* user = malloc(sizeof(*user));
  ASSERT(user, "Couldn't allocate memory properly");

  user->conn = conn;
  user->pinged = 0;
  user->channels_count = 0;
  memset(user->realname, '\0', YI_MAX_NAME);
  memset(user->nickname, '\0', YI_MAX_NAME);
  memset(user->username, '\0', YI_MAX_NAME);

  return user;
}

void yi_user_destroy(yi_user_t* user)
{
  yi_connection_destroy(user->conn);
  FREE(user);
}
