#include <string.h>
#include <stdio.h>

#include "yairc/common.h"
#include "yairc/parser.h"

void test1()
{
  const char* msg = "PING :levin.mozilla.org\r\n";
  yi_message_t* message = yi_parse(msg, strlen(msg));

  ASSERT(strlen(message->prefix) == 0, "No prefix");
  STRNCMP(message->command, "PING");
  STRNCMP(message->parameters[0], ":levin.mozilla.org");

  yi_message_destroy(message);
}

int main(int argc, char* argv[])
{
  TEST(test1);

  return 0;
}
