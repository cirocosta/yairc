#ifndef YI__CLIENT_H
#define YI__CLIENT_H 

#include "yairc/connection.h"
#include <linux/limits.h>

/**
 * A client has:
 *  - connection
 *  - nickname
 *  - username
 *  - realname
 *  - list of channels 
 */

typedef struct yi_user_t {
  yi_connection_t* conn;
  char realname[NAME_MAX];
  char nickname[NAME_MAX];
  char username[NAME_MAX];
  unsigned cids[15];
} yi_user_t;

#endif
