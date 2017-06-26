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

  int pinged;
} yi_user_t;

yi_user_t* yi_user_create(yi_connection_t* conn);
void yi_user_destroy(yi_user_t* user);

#endif
