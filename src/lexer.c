#include "yairc/lexer.h"

yi_buffer_t* yi_buffer_create(const char* str, size_t buf_len)
{
  yi_buffer_t* buf = malloc(sizeof(*buf));
  yi_token_t* token = malloc(sizeof(*token));

  PASSERT(buf, "yi_buffer_new:");
  PASSERT(token, "yi_buffer_new:");

  buf->token = token;
  yi_buffer_init(buf, str, buf_len);

  return buf;
}

void yi_buffer_init(yi_buffer_t* buf, const char* str, size_t buf_len)
{
  buf->buf = str;
  buf->la = buf->buf;
  buf->pos = 0;
  buf->size = buf_len;
}

void yi_buffer_destroy(yi_buffer_t* buf)
{
  free(buf->token);
  FREE(buf);
}

inline static void yi_buffer_update(yi_buffer_t* buf, char const* peek,
                                    yi_token_type_e type)
{
  buf->token->type = type;
  buf->token->len = peek - buf->la;
  memcpy(buf->token->buf, buf->la, buf->token->len);
  buf->la = peek;
}

int yi_lex_single_terminal(yi_buffer_t* buf, char c)
{
  char const* peek;

  if (!(peek = _is_single_terminal(buf->la, c)))
    return 0;

  yi_buffer_update(buf, peek, YI_T_SINGLE_TERMINAL);

  return 1;
}

int yi_lex_terminal(yi_buffer_t* buf, char* terminal, unsigned size)
{
  char const* peek;

  if (!(peek = _is_terminal(buf->la, terminal, size)))
    return 0;

  yi_buffer_update(buf, peek, YI_T_TERMINAL);

  return 1;
}

int yi_lex_hostname(yi_buffer_t* buf)
{
  char const* peek;

  if (!(peek = _is_hostname(buf->la)))
    return 0;

  yi_buffer_update(buf, peek, YI_T_HOSTNAME);

  return 1;
}

int yi_lex_host(yi_buffer_t* buf)
{
  char const* peek;

  if ((peek = _is_hostname(buf->la)) || (peek = _is_ip4addr(buf->la)) ||
      (peek = _is_ip6addr(buf->la))) {
    yi_buffer_update(buf, peek, YI_T_HOST);
    return 1;
  }

  return 0;
}

int yi_lex_user(yi_buffer_t* buf)
{
  char const* peek;

  if (!(peek = _is_user(buf->la)))
    return 0;

  yi_buffer_update(buf, peek, YI_T_USER);

  return 1;
}

int yi_lex_command(yi_buffer_t* buf)
{
  char const* peek;

  if (!(peek = _is_command(buf->la)))
    return 0;

  yi_buffer_update(buf, peek, YI_T_COMMAND);

  return 1;
}

int yi_lex_nickname(yi_buffer_t* buf)
{
  char const* peek;

  if (!(peek = _is_command(buf->la)))
    return 0;

  yi_buffer_update(buf, peek, YI_T_COMMAND);

  return 1;
}

int yi_lex_params(yi_buffer_t* buf)
{
  char const* peek = buf->la;
  unsigned remaining_middle_params = 14;

  while (remaining_middle_params-- > 0) {
    if (!(peek = _is_single_terminal(peek, ' ')) &&
        !(peek = _is_middle(peek)))
      break;
  }

  if ((peek = _is_terminal(peek, " :", 2))) {
    if (!(peek = _is_trailing(peek)))
      return 0;
  }

  return 1;
}
