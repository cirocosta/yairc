#ifndef YI__COMMANDS_H
#define YI__COMMANDS_H

#include <string.h>
#include <stdio.h>

enum yi_command_e {
  YI_COMM_PASS = 1,
  YI_COMM_NICK, //
  YI_COMM_USER, //
  YI_COMM_MODE,
  YI_COMM_SERVICE,
  YI_COMM_QUIT, //
  YI_COMM_SQUIT,

  YI_COMM_JOIN, //
  YI_COMM_PART,
  YI_COMM_CHAN_MODE,
  YI_COMM_TOPIC,
  YI_COMM_NAMES,
  YI_COMM_LIST, //
  YI_COMM_INVITE,
  YI_COMM_KICK,
  YI_COMM_PRIVMSG,
  YI_COMM_NOTICE,
  YI_COMM_MOTD,
  YI_COMM_LUSERS,
  YI_COMM_VERSION,
  YI_COMM_STATS,
  YI_COMM_LINKS,
  YI_COMM_TIME,
  YI_COMM_CONNECT,
  YI_COMM_TRACE,
  YI_COMM_ADMIN,
  YI_COMM_INFO,
  YI_COMM_SERVLIST,
  YI_COMM_SQUERY,

  YI_COMM_PING,
  YI_COMM_PONG,
  YI_COMM_ERROR,
};

enum yi_command_custom_e {
  YI_COMM_MACDATA,
  YI_COMM_MACHORA,
  YI_COMM_MACTEMPERATURA,
};

enum yi_reply_e {
  YI_RPL_WELCOME = 1,
  YI_RPL_YOURSERVICE,
};

enum yi_command_type { YI_NUMERIC_REPLY, YI_NAMED_COMMAND };

void yi_command_NICK(char* buf, char* nickname)
{
  char* cmd_str = "NICK %s";

  snprintf(buf, 512, cmd_str, nickname);
}

void yi_command_USER(char* buf, char* username, char* mode, char* realname)
{
  char* cmd_str = "USER %s %s * :%s";
  snprintf(buf, 512, cmd_str, username, mode, realname);
}

#endif
