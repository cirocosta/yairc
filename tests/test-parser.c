#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "yairc/common.h"
#include "yairc/parser.h"

void test1()
{
  const char* msg = "PING :levin.mozilla.org\r\n";
  yi_message_t* message = yi_parse(msg, strlen(msg));

  ASSERT(strlen(message->prefix) == 0, "No prefix");
  STRNCMP(message->command, "PING");
  STRNCMP(message->parameters[0], "levin.mozilla.org");

  yi_message_reset(message, msg, strlen(msg));
  ASSERT(strlen(message->prefix) == 0, "No prefix");
  ASSERT(strlen(message->command) == 0, "No command");
  ASSERT(strlen(message->parameters[0]) == 0, "No first parameter");

  yi_parse_m(message, msg, strlen(msg));
  ASSERT(strlen(message->prefix) == 0, "No prefix");
  STRNCMP(message->command, "PING");
  STRNCMP(message->parameters[0], "levin.mozilla.org");

  yi_message_destroy(message);
}

void test2()
{
  const char* msg =
      ":levin.mozilla.org 376 guest :End of message of the day.\r\n";
  yi_message_t* message = yi_parse(msg, strlen(msg));

  STRNCMP(message->prefix, "levin.mozilla.org");
  STRNCMP(message->command, "376");
  STRNCMP(message->parameters[0], "guest");
  STRNCMP(message->parameters[1], "End of message of the day.");

  // resets internally and is ready to do it again
  yi_parse_m(message, msg, strlen(msg));
  STRNCMP(message->prefix, "levin.mozilla.org");
  STRNCMP(message->command, "376");
  STRNCMP(message->parameters[0], "guest");
  STRNCMP(message->parameters[1], "End of message of the day.");

  yi_message_destroy(message);
}

void test3()
{
  const char* msg = ":NickServ!services@ircservices.mozilla.org NOTICE guest "
                    ":please choose a different nick.\r\n";
  const char* msg2 =
      ":levin.mozilla.org 376 guest :End of message of the day.\r\n";

  yi_message_t* message = yi_message_create(NULL, 0);

  yi_parse_m(message, msg, strlen(msg));

  STRNCMP(message->prefix, "NickServ!services@ircservices.mozilla.org");
  STRNCMP(message->command, "NOTICE");
  STRNCMP(message->parameters[0], "guest");
  STRNCMP(message->parameters[1], "please choose a different nick.");

  // multiple messages

  yi_parse_m(message, msg2, strlen(msg2));

  STRNCMP(message->prefix, "levin.mozilla.org");
  STRNCMP(message->command, "376");
  STRNCMP(message->parameters[0], "guest");
  STRNCMP(message->parameters[1], "End of message of the day.");

  yi_message_destroy(message);
}

int main(int argc, char* argv[])
{
  TEST(test1);
  TEST(test2);
  TEST(test3);

  return 0;
}
