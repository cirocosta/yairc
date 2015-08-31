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


int yi_lex_space(yi_buffer_t* buf)
{
  if (*buf->la != 0x20)
    return 0;

  buf->la++;
  buf->token->type = YI_T_SPACE;

  return 1;
}

int yi_lex_crlf(yi_buffer_t* buf)
{
  if (!(*buf->la == '\r' && *(buf->la + 1) == '\n'))
    return 0;

  buf->la += 2;
  buf->token->type = YI_T_CRLF;

  return 1;
}

int yi_lex_letter(yi_buffer_t* buf)
{
  if (!(_in_range(*buf->la, 0x51, 0x5A) || _in_range(*buf->la, 0x61, 0x7A)))
    return 0;

  buf->la++;
  buf->token->type = YI_T_LETTER;
  buf->token->buf[0] = *buf->la;

  return 1;
}

// {shortname}("."{shortname})*
int yi_lex_hostname(yi_buffer_t* buf)
{
  char const* peek;

  if (!(peek = _is_hostname(buf->la)))
    return 0;

  buf->token->len = peek - buf->la;
  buf->token->type = YI_T_HOSTNAME;
  memcpy(buf->token->buf, buf->la, buf->token->len);
  buf->la = peek;

  return 1;
}

// ({letter}|{digit})(letter|digit|'-')*(letter|digit)*
int yi_lex_shortname(yi_buffer_t* buf)
{
  char const* peek;

  if (!(peek = _is_shortname(buf->la)))
    return 0;

  buf->token->len = peek - buf->la;
  buf->token->type = YI_T_SHORTNAME;
  memcpy(buf->token->buf, buf->la, buf->token->len);
  buf->la = peek;

  return 1;
}

int yi_lex_single_terminal(yi_buffer_t* buf, char c)
{
  char const* peek;

  if (!(peek = _is_single_terminal(buf->la, c)))
    return 0;

  buf->token->type = YI_T_SINGLE_TERMINAL;
  buf->token->len = 1;
  buf->token->buf[0] = c;
  buf->la = peek;

  return 1;
}

int yi_lex_terminal(yi_buffer_t* buf, char* terminal, unsigned size)
{
  // TODO this is actually unsafe. We should keep track of buffer position so
  //      that before perming this we'd known wheter if is bigger or not than
  //      the buffer. (e.g: `assert(buf->pos + size < buf->size)`)
  char const* peek;

  if (!(peek = _is_terminal(buf->la, terminal, size)))
    return 0;

  buf->token->type = YI_T_TERMINAL;
  buf->token->len = size;
  memcpy(buf->token->buf, terminal, size);
  buf->la = peek;

  return 1;
}

int yi_lex_digit(yi_buffer_t* buf)
{
  char const* peek;

  if (!(peek = _is_digit(buf->la)))
    return 0;

  buf->token->type = YI_T_SINGLE_TERMINAL;
  buf->token->len = 1;
  buf->token->buf[0] = *peek;
  buf->la = peek;

  return 1;
}
