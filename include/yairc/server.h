#ifndef YI__SERVER_H
#define YI__SERVER_H 

#include <linux/limits.h>
#include "yairc/user.h"
#include "yairc/channel.h"

typedef struct yi_server_t
{
  char name[NAME_MAX];
  yi_user_t* users[100];
  yi_channel_t* channels[15];
} yi_server_t;

#endif
