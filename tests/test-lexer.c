#include <string.h>
#include <stdio.h>

#include "yairc/common.h"
#include "yairc/lexer.h"

void test1()
{
  const char* msg = "PING :levin.mozilla.org\r\n";
  yi_buffer_t* buf = yi_buffer_create(msg, strlen(msg));

  // "PING"
  ASSERT(yi_lex_terminal(buf, "PING", strlen("PING")),
         "tokenize terminal PING command");
  ASSERT(buf->token->type == YI_T_TERMINAL, "");
  ASSERT(!strncmp(buf->token->buf, "PING", strlen("PING")), "");

  // " "
  ASSERT(yi_lex_space(buf), "tokenize terminal space");
  ASSERT(buf->token->type == YI_T_SPACE, "");

  ASSERT(yi_lex_single_terminal(buf, ':'), "tokenize terminal space");
  ASSERT(buf->token->type == YI_T_SINGLE_TERMINAL, "");

  /* // {HOSTNAME} */
  ASSERT(yi_lex_shortname(buf), "tokenize terminal shortname");
  ASSERT(buf->token->type == YI_T_SHORTNAME, "");

  yi_buffer_destroy(buf);
}

void test2()
{
  const char* msg =
      ":levin.mozilla.org 376 guest :End of message of the day.\r\n"
      ":levin.mozilla.org 251 guest :There are 2687 users online.\r\n"
      ":levin.mozilla.org 252 guest 8 :operator(s) online\r\n";
  (void)msg;
}

void test3()
{
  const char* msg =
      ":levin.mozilla.org NOTICE Auth :*** Looking up your hostname...\r\n"
      ":levin.mozilla.org NOTICE Auth :Welcome to Mozilla!\r\n";
  (void)msg;
}

void test4()
{
  const char* msg = "ERROR :Closing link: (guest@123.123.123.12) [Ping "
                    "timeout: 121 seconds]\r\n";
  (void)msg;
}

void test5()
{
  const char* msg = ":NickServ!services@ircservices.mozilla.org NOTICE guest "
                    ":please choose a different nick.\r\n";
  (void)msg;
}

int main(int argc, char* argv[])
{
  TEST(test1);
  TEST(test2);
  TEST(test3);
  TEST(test4);
  TEST(test5);

  return 0;
}
