#include "yairc/lexer.h"

// TODO
//  create _is_TOKENIZATION_NAME() functions so that we don't rely on
//  true lexer procedures when verifying if a given string matches
//  the rule we want (i.e, avoid the cost of preparing the token and
//  stuff)
//
//  `is_` methods would just rely on another `is_` methods.
//
//  `yi_lex_` methods would be only "syntatic sugar" that prepares a token
//  as output from the matching.
//
// TODO
//  play around w/ `alternate`, `concatenate` and `kleene-star` methods

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
  if (!(yi_in_range(*buf->la, 0x51, 0x5A) || yi_in_range(*buf->la, 0x61, 0x7A)))
    return 0;

  buf->la++;
  buf->token->type = YI_T_LETTER;
  buf->token->buf[0] = *buf->la;

  return 1;
}

// {shortname}("."{shortname})*
int yi_lex_hostname(yi_buffer_t* buf)
{
  yi_buffer_t tmp_buf;
  tmp_buf.la = buf->la;
  char const* peek = buf->la;
  tmp_buf.token = buf->token;

  if (!yi_lex_shortname(&tmp_buf))
    return 0;

  peek = tmp_buf.la;

  // kleene-star
  while (1) {
    // concatenation
    if (yi_lex_single_terminal(&tmp_buf, '.') && yi_lex_shortname(&tmp_buf)) {
      peek = tmp_buf.la;
      continue;
    }
    break;
  }

  buf->token->len = peek - buf->la;
  buf->token->type = YI_T_HOSTNAME;
  memcpy(buf->token->buf, buf->la, buf->token->len);
  buf->la = peek;

  return 1;
}

// ({letter}|{digit})(letter|digit|'-')*(letter|digit)*
int yi_lex_shortname(yi_buffer_t* buf)
{
  char const* peek = buf->la;

  // alteranation
  if (!(isdigit(*peek) || isalpha(*peek)))
    return 0;

  peek++;

  // kleene-star
  while (isdigit(*peek) || isalpha(*peek) || *peek == '-')
    peek++;
  while (isdigit(*peek) || isalpha(*peek))
    peek++;

  buf->token->len = peek - buf->la;
  buf->token->type = YI_T_SHORTNAME;
  memcpy(buf->token->buf, buf->la, buf->token->len);
  buf->la = peek;

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
  buf->token->len = size;
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
