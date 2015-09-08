#include "yairc/parser.h"

// TODO deal with the case of malformed messages.

inline static yi_message_status_e _MESSAGE(yi_message_t* message);
inline static yi_message_status_e _PREFIX(yi_message_t* message);
inline static yi_message_status_e _COMMAND(yi_message_t* message);
inline static yi_message_status_e _PARAMETERS(yi_message_t* message);

yi_message_status_e yi_parse_m(yi_message_t* message, const char* src,
                             unsigned src_size)
{
  yi_message_reset(message, src, src_size);

  return _MESSAGE(message);
}

yi_message_t* yi_message_create(const char* msg, unsigned msg_size)
{
  unsigned i = 0;

  yi_message_t* message = malloc(sizeof(*message));
  PASSERT(message, "yi_message_create:");

  if (msg)
    message->buf = yi_buffer_create(msg, msg_size);
  else
    message->buf = yi_buffer_create("", 0);

  message->prefix[0] = '\0';
  message->command[0] = '\0';
  while (i < YI_MAX_MESSAGE_PARAMS)
    message->parameters[i++][0] = '\0';

  return message;
}

void yi_message_reset(yi_message_t* message, const char* msg, unsigned msg_size)
{
  unsigned i = 0;

  yi_buffer_reset(message->buf, msg, msg_size);

  message->prefix[0] = '\0';
  message->command[0] = '\0';
  while (i < YI_MAX_MESSAGE_PARAMS)
    message->parameters[i++][0] = '\0';
}

void yi_message_destroy(yi_message_t* message)
{
  yi_buffer_destroy(message->buf);
  FREE(message);
}

inline static yi_message_status_e _MESSAGE(yi_message_t* message)
{
  if (yi_lex_terminal(message->buf, "\r\n", 2))
    return YI_MESSAGE_NUL;

  _PREFIX(message);
  _COMMAND(message);
  _PARAMETERS(message);

  yi_lex_terminal(message->buf, "\r\n", 2);

  return YI_MESSAGE_OK;
}

inline static yi_message_status_e _PREFIX(yi_message_t* message)
{
  if (yi_lex_prefix(message->buf)) {
    strncpy(message->prefix, message->buf->token->buf,
            message->buf->token->len);
    message->prefix[message->buf->token->len] = '\0';
  }

  return YI_MESSAGE_OK;
}

inline static yi_message_status_e _COMMAND(yi_message_t* message)
{
  yi_lex_command(message->buf);
  strncpy(message->command, message->buf->token->buf, message->buf->token->len);
  message->command[message->buf->token->len] = '\0';

  return YI_MESSAGE_OK;
}

inline static yi_message_status_e _PARAMETERS(yi_message_t* message)
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

  return YI_MESSAGE_OK;
}
