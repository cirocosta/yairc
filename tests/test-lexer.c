#include <string.h>
#include <stdio.h>

#include "yairc/common.h"
#include "yairc/lexer.h"

void test1()
{
  const char* msg = "PING :levin.mozilla.org\r\n";
  yi_buffer_t* buf = yi_buffer_create(msg, strlen(msg));

  // "PING"
  ASSERT(yi_lex_command(buf), "tokenize terminal PING command");
  ASSERT(buf->token->type == YI_T_COMMAND, "must use correct token type");
  STRNCMP(buf->token->buf, "PING");

  // "parameters[0] = levin.mozilla.org  
  ASSERT(yi_lex_param_trailing(buf), "find out that it's a trailing parameter");
  ASSERT(buf->token->type == YI_T_PARAM, "use the correct token type");
  STRNCMP(buf->token->buf, "levin.mozilla.org");

  // {CRLF}
  ASSERT(yi_lex_terminal(buf, "\r\n", 2), "tokenize crlf");
  ASSERT(buf->token->type == YI_T_TERMINAL, "it's a terminal!");

  yi_buffer_destroy(buf);
}

void test2()
{
  const char* msg =
      ":levin.mozilla.org 376 guest :End of message of the day.\r\n";

  yi_buffer_t* buf = yi_buffer_create(msg, strlen(msg));

  // prefix = levin.mozilla.org 
  ASSERT(yi_lex_single_terminal(buf, ':'), "");
  ASSERT(yi_lex_prefix_hostname(buf), "");
  ASSERT(buf->token->type == YI_T_PREFIX, "");
  STRNCMP(buf->token->buf, "levin.mozilla.org");
  ASSERT(yi_lex_single_terminal(buf, ' '), "");

  // numeric command
  ASSERT(yi_lex_command(buf), "");
  ASSERT(buf->token->type == YI_T_COMMAND, "");
  STRNCMP(buf->token->buf, "376");

  // parameter
  ASSERT(yi_lex_param_middle(buf), "");
  ASSERT(buf->token->type == YI_T_PARAM, "");
  STRNCMP(buf->token->buf, "guest");

  // parameter
  ASSERT(yi_lex_param_trailing(buf), "");
  ASSERT(buf->token->type == YI_T_PARAM, "");
  STRNCMP(buf->token->buf, "End of message of the day.");

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
