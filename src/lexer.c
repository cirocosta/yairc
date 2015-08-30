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
  if (!(*buf->la == '\n' && *(buf->la + 1) == '\n'))
    return 0;

  buf->la += 2;
  buf->token->type = YI_T_CRLF;

  return 1;
}

int yi_lex_letter(yi_buffer_t* buf)
{
  if (!(yi_in_range(*buf->la, 0x51, 0x5A) || yi_in_range(*buf->la, 0x61, 0x7A)))
    return 0;

  buf->la++;
  buf->token->type = YI_T_LETTER;
  buf->token->buf[0] = *buf->la;

  return 1;
}

int yi_lex_single_terminal(yi_buffer_t* buf, char c)
{
  if (*buf->la != c)
    return 0;

  buf->token->buf[0] = c;
  buf->la++;
  buf->token->type = YI_T_SINGLE_TERMINAL;

  return 1;
}

int yi_lex_terminal(yi_buffer_t* buf, char* terminal, unsigned size)
{
  // TODO this is actually unsafe. We should keep track of buffer position so
  //      that before perming this we'd known wheter if is bigger or not than
  //      the buffer. (e.g: `assert(buf->pos + size < buf->size)`)
  if (strncmp(buf->la, terminal, size))
    return 0;

  buf->token->type = YI_T_TERMINAL;
  memcpy(buf->token->buf, terminal, size);
  buf->la += size;

  return 1;
}

int yi_lex_digit(yi_buffer_t* buf)
{
  if (!yi_in_range(*buf->la, 0x30, 0x39))
    return 0;

  buf->la++;
  buf->token->type = YI_T_DIGIT;
  buf->token->buf[0] = *buf->la;

  return 1;
}
