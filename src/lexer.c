#include "yairc/lexer.h"

yi_buffer_t* yi_buffer_create(const char* str, size_t buf_len)
{
  yi_buffer_t* buf = malloc(sizeof(*buf));
  yi_token_t* token = malloc(sizeof(*token));

  PASSERT(buf, "yi_buffer_create:");
  PASSERT(token, "yi_buffer_create:");

  buf->token = token;
  yi_buffer_reset(buf, str, buf_len);

  return buf;
}

void yi_buffer_reset(yi_buffer_t* buf, const char* str, size_t buf_len)
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
  buf->token->buf[buf->token->len] = '\0';
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

int yi_lex_prefix_hostname(yi_buffer_t* buf)
{
  char const* peek;

  if (!(peek = _is_hostname(buf->la)))
    return 0;

  yi_buffer_update(buf, peek, YI_T_PREFIX);

  return 1;
}

int yi_lex_prefix_nickname(yi_buffer_t* buf)
{
  char const* peek;
  char const* tmp;

  if (!(peek = _is_nickname(buf->la)))
    return 0;

  tmp = peek;
  
  if ((tmp = _is_single_terminal(tmp, '!'))) {
    if (!(tmp = _is_user(tmp)))
      return 0;
    if (!(tmp = _is_single_terminal(tmp, '@')))
      return 0;
    if (!(tmp = _is_host(tmp)))
      return 0;
    peek = tmp;
  } 
  
  else if ((tmp = _is_single_terminal(tmp, '@'))) {
    if (!(tmp = _is_host(tmp)))
      return 0;
    peek = tmp;
  }

  yi_buffer_update(buf, peek, YI_T_PREFIX);

  return 1;
}

int yi_lex_command(yi_buffer_t* buf)
{
  char const* peek = buf->la;

  if (!(peek = _is_command(peek)))
    return 0;

  yi_buffer_update(buf, peek, YI_T_COMMAND);

  return 1;
}

// " "{middle}
int yi_lex_param_middle(yi_buffer_t* buf)
{
  char const* peek = buf->la;

  if (!(peek = _is_single_terminal(peek, ' ')))
    return 0;

  if (!(peek = _is_middle(peek)))
    return 0;

  buf->la += 1;
  yi_buffer_update(buf, peek, YI_T_PARAM);

  return 1;
}

// " :"{trailing}
int yi_lex_param_trailing(yi_buffer_t* buf)
{
  char const* peek = buf->la;

  if (!(peek = _is_terminal(peek, " :", 2)))
    return 0;

  if (!(peek = _is_trailing(peek)))
    return 0;

  buf->la += 2;
  yi_buffer_update(buf, peek, YI_T_PARAM);

  return 1;
}

#if 0
int yi_lex_params(yi_buffer_t* buf)
{
  char const* peek = buf->la;
  char const* tmp = peek;
  unsigned remaining_middle_params = 14;

  while (remaining_middle_params-- > 0) {
    if (!(tmp = _is_single_terminal(peek, ' ')))
      break;
    if (!(tmp = _is_middle(tmp)))
      break;
    peek = tmp;
  }

  if ((peek = _is_terminal(peek, " :", 2))) {
    if (!(peek = _is_trailing(peek)))
      return 0;
  }

  yi_buffer_update(buf, peek, YI_T_COMMAND);

  return 1;
}
#endif
