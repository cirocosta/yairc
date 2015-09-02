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
  STRNCMP(buf->token->buf, "PING");


  // " "
  ASSERT(yi_lex_single_terminal(buf, ' '), "tokenize terminal space");
  ASSERT(buf->token->type == YI_T_SINGLE_TERMINAL, "");

  ASSERT(yi_lex_single_terminal(buf, ':'), "tokenize terminal space");
  ASSERT(buf->token->type == YI_T_SINGLE_TERMINAL, "");

  // {HOSTNAME}
  ASSERT(yi_lex_hostname(buf), "tokenize terminal hostname");
  ASSERT(buf->token->type == YI_T_HOSTNAME, "");
  STRNCMP(buf->token->buf, "levin.mozilla.org");

  // {CRLF}
  ASSERT(yi_lex_terminal(buf, "\r\n", 2), "tokenize crlf");
  ASSERT(buf->token->type == YI_T_TERMINAL, "");

  yi_buffer_destroy(buf);
}

void test2()
{
  const char* msg =
      ":levin.mozilla.org 376 guest :End of message of the day.\r\n"
      ":levin.mozilla.org 251 guest :There are 2687 users online.\r\n"
      ":levin.mozilla.org 252 guest 8 :operator(s) online\r\n";

  yi_buffer_t* buf = yi_buffer_create(msg, strlen(msg));

  yi_buffer_destroy(buf);
}

void test3()
{
  const char* msg =
      ":levin.mozilla.org NOTICE Auth :*** Looking up your hostname...\r\n"
      ":levin.mozilla.org NOTICE Auth :Welcome to Mozilla!\r\n";

  yi_buffer_t* buf = yi_buffer_create(msg, strlen(msg));

  yi_buffer_destroy(buf);
}

void test4_0()
{
  unsigned i = 4;
  char* msg[] = { "123.123.123.12", "255.255.255.255", "127.0.0.1",
                  "10.10.10.10" };
  while (i-- > 0) {
    ASSERT(_is_ip4addr(msg[i]), "%s is an ip4addr", msg[i]);
  }
}

void test4()
{
  const char* msg = "ERROR :Closing link: (guest@123.123.123.12) [Ping "
                    "timeout: 121 seconds]\r\n";

  yi_buffer_t* buf = yi_buffer_create(msg, strlen(msg));

  yi_buffer_destroy(buf);
}

void test5()
{
  const char* msg = ":NickServ!services@ircservices.mozilla.org NOTICE guest "
                    ":please choose a different nick.\r\n";

  yi_buffer_t* buf = yi_buffer_create(msg, strlen(msg));

  yi_buffer_destroy(buf);
}


int main(int argc, char* argv[])
{
  TEST(test1);
  TEST(test2);
  TEST(test3);
  TEST(test4_0);
  TEST(test4);
  TEST(test5);

  return 0;
}
