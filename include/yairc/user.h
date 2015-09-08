#ifndef YI__CLIENT_H
#define YI__CLIENT_H

#include "yairc/constants.h"
#include "yairc/connection.h"
#include "yairc/common.h"

typedef struct yi_user_t {
  yi_connection_t* conn;
  char realname[YI_MAX_NAME];
  char nickname[YI_MAX_NAME];
  char username[YI_MAX_NAME];
  unsigned cids[YI_MAX_CHANNELS];
  unsigned uid;
  unsigned channels_count;
} yi_user_t;

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
  FREE(user);
}

#endif
