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

// FIXME we're always receiving ("", 0) - and so, the same for the
//       buffers that's being created. Simplify this.
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
    message->parameters_count++;
    strncpy(message->parameters[i], message->buf->token->buf,
            message->buf->token->len);
    message->parameters[i][message->buf->token->len] = '\0';
    i++;
  }

  if (yi_lex_param_trailing(message->buf)) {
    message->parameters_count++;
    strncpy(message->parameters[i], message->buf->token->buf,
            message->buf->token->len);
    message->parameters[i][message->buf->token->len] = '\0';
  }

  return YI_MESSAGE_OK;
}

void yi_message_parse_fd(int fd, void* data,
                         void (*process_message)(void* data, yi_message_t* msg))
{
  yi_message_t* message = yi_message_create(NULL, 0);
  char buf[YI_MAX_LINE] = { 0 };
  char out_buf[YI_MAX_MESSAGE] = { 0 };
  char* la = NULL;
  char* tmp = NULL;

  int nread = 0;
  unsigned tot_read = 0;
  unsigned len = 0;

  while (1) {
    if ((nread = read(fd, buf + tot_read, YI_MAX_LINE - tot_read)) < 0) {
      perror("yi_read_incoming::read error:");
      break;
    } else if (!nread) {
      break;
    }

    tot_read += nread;
    tmp = buf;
    la = strstr(tmp, CRLF);

    if (!la)
      continue;

    do {
      la += 2;
      len = la - tmp;

      memcpy(out_buf, tmp, len);
      out_buf[len] = '\0';
      tmp = la;
      tot_read -= len;

      if (yi_parse_m(message, out_buf, len) == YI_MESSAGE_NUL)
        continue;

      process_message(data, message);
    } while ((la = strstr(tmp, CRLF)));

    memmove(buf, tmp, strlen(tmp));
    buf[strlen(tmp)] = '\0';
  }

  yi_message_destroy(message);
}
