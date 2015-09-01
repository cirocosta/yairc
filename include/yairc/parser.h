#ifndef YI__PARSER_H
#define YI__PARSER_H

#include "yairc/lexer.h"

typedef struct yi_message_t {
  char* prefix;
  char* command;
  char** parameters;
} yi_message_t;

inline static void _MESSAGE(yi_buffer_t* buf);
inline static void _PREFIX(yi_buffer_t* buf);
inline static void _COMMAND(yi_buffer_t* buf);
inline static void _PARAMETERS(yi_buffer_t* buf);

yi_message_t* yi_message_create()
{
  yi_message_t* message = malloc(sizeof(*message));
  PASSERT(message, "yi_message_create:");

  return message;
}

void yi_message_destroy(yi_message_t* message)
{
  FREE(message);
}

yi_message_t* yi_parse(const char* msg, unsigned msg_size)
{
  yi_message_t* message = yi_message_create();
  yi_buffer_t* buf = yi_buffer_create(msg, msg_size);

  _MESSAGE(buf);

  yi_buffer_destroy(buf);

  return message;
}

inline static void _MESSAGE(yi_buffer_t* buf)
{
  if (yi_lex_single_terminal(buf, ':')) {
    _PREFIX(buf); // opt
    yi_lex_single_terminal(buf, ' ');
    LOG("prefix: %s", buf->token->buf);
  }

  _COMMAND(buf);
  LOG("command: %s", buf->token->buf);
  _PARAMETERS(buf);
  LOG("params: %s", buf->token->buf);
  yi_lex_terminal(buf, "\r\n", 2);
}

inline static void _PREFIX(yi_buffer_t* buf)
{
  if (yi_lex_hostname(buf))
    return;

  if (!yi_lex_nickname(buf))
    exit(1); // err

  if (yi_lex_single_terminal(buf, '!')) {
    yi_lex_user(buf);
    yi_lex_single_terminal(buf, '@');
    yi_lex_host(buf);
  } else if (yi_lex_single_terminal(buf, '@')) {
    yi_lex_host(buf);
  }

}

inline static void _COMMAND(yi_buffer_t* buf)
{
  yi_lex_command(buf);
}

inline static void _PARAMETERS(yi_buffer_t* buf)
{
  if (!yi_lex_params(buf))
    LOGERR("_PARAMETERS");
}

#endif
