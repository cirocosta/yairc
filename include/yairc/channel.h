#ifndef YI__CHANNEL_H
#define YI__CHANNEL_H

#include <linux/limits.h>
#include "yairc/user.h"

typedef struct yi_channel_t {
  char name[NAME_MAX];
  char topic[NAME_MAX];
  unsigned cid;
  yi_user_t* users[100];
} yi_channel_t;

yi_channel_t* yi_channel_create();

#endif
