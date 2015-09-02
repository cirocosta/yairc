#ifndef YI__PARSER_H
#define YI__PARSER_H

#include "yairc/lexer.h"

typedef struct yi_message_t {
  yi_buffer_t* buf;

  char prefix[513];
  char command[513];
  char parameters[15][513]; // TODO wow!
} yi_message_t;

inline static void _MESSAGE(yi_message_t* message);
inline static void _PREFIX(yi_message_t* message);
inline static void _COMMAND(yi_message_t* message);
inline static void _PARAMETERS(yi_message_t* message);

yi_message_t* yi_message_create(const char* msg, unsigned msg_size)
{
  unsigned i = 0;

  yi_message_t* message = malloc(sizeof(*message));
  PASSERT(message, "yi_message_create:");

  message->buf = yi_buffer_create(msg, msg_size);

  message->prefix[0] = '\0';
  message->command[0] = '\0';
  while (i < 15)
    message->parameters[i++][0] = '\0';

  return message;
}

void yi_message_reset(yi_message_t* message, const char* msg, unsigned msg_size)
{
  unsigned i = 0;

  yi_buffer_reset(message->buf, msg, msg_size);

  message->prefix[0] = '\0';
  message->command[0] = '\0';
  while (i < 15)
    message->parameters[i++][0] = '\0';
}

void yi_message_destroy(yi_message_t* message)
{
  yi_buffer_destroy(message->buf);
  FREE(message);
}

yi_message_t* yi_parse(const char* msg, unsigned msg_size)
{
  yi_message_t* message = yi_message_create(msg, msg_size);

  _MESSAGE(message);

  return message;
}

inline static void _MESSAGE(yi_message_t* message)
{
  _PREFIX(message);
  _COMMAND(message);
  _PARAMETERS(message);
  yi_lex_terminal(message->buf, "\r\n", 2);
}

inline static void _PREFIX(yi_message_t* message)
{
  if (!yi_lex_single_terminal(message->buf, ':'))
    return;

  if (yi_lex_prefix_hostname(message->buf)) {
    strncpy(message->prefix, message->buf->token->buf,
            message->buf->token->len);
  } else if (yi_lex_prefix_nickname(message->buf)) {
    strncpy(message->prefix, message->buf->token->buf,
            message->buf->token->len);
  }

  yi_lex_single_terminal(message->buf, ' ');
}

inline static void _COMMAND(yi_message_t* message)
{
  yi_lex_command(message->buf);
  strncpy(message->command, message->buf->token->buf, message->buf->token->len);
}

inline static void _PARAMETERS(yi_message_t* message)
{
  unsigned i = 0;

  while (i < 14) {
    if (!yi_lex_param_middle(message->buf))
      break;
    strncpy(message->parameters[i], message->buf->token->buf,
            message->buf->token->len);
    message->parameters[i][message->buf->token->len] = '\0';
    i++;
  }

  if (yi_lex_param_trailing(message->buf)) {
    strncpy(message->parameters[i], message->buf->token->buf,
            message->buf->token->len);
    message->parameters[i][message->buf->token->len] = '\0';
  }
}

#endif
