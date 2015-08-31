#include "yairc/lexer.h"

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

static inline char const* _is_single_terminal(char const* la, char c)
{
  return *la == c ? ++la : NULL;
}

static inline char const* _is_digit(char const* la)
{
  if (!_in_range(*la, 0x30, 0x39))
    return NULL;

  return la + 1;
}

static char const* _is_shortname(char const* peek)
{
  // alteranation
  if (!(isdigit(*peek) || isalpha(*peek)))
    return NULL;

  peek++;

  // kleene-star
  while (isalpha(*peek) || isdigit(*peek) || *peek == '-')
    peek++;
  while (isalpha(*peek) || isdigit(*peek))
    peek++;

  return peek;
}

static char const* _is_hostname(char const* peek)
{
  char const* tmp_peek;

  if (!(peek = _is_shortname(peek)))
    return NULL;

  tmp_peek = peek;

  // kleene-star
  while (*peek) {
    // concatenation
    if ((tmp_peek = _is_single_terminal(tmp_peek, '.')) &&
        (tmp_peek = _is_shortname(tmp_peek))) {
      peek = tmp_peek;
      continue;
    }
    break;
  }

  return peek;
}

static char const* _is_terminal(char const* peek, char* terminal, unsigned size)
{
  if (strncmp(peek, terminal, size))
    return NULL;

  return peek + size;
}

// nickname = (letter|special) *8(letter|digit|special|"-")
// (letter|special) --reduce--> [0x41-0x7D]
// so: [0x41-0x7D]*8([0x41-0x7D]|digit|"-")
static char const* _is_nickname(char const* peek)
{
  unsigned max_it = 8;

  if (!_in_range(*peek, 0x41, 0x7D))
    return NULL;
  peek++;

  while (max_it-- > 0) {
    if (_in_range(*peek, 0x41, 0x7D) || isdigit(*peek) || *peek == '-')
      peek++;
    else
      break;
  }

  return peek;
}

// 1*( %x01-09 / %x0B-0C / %x0E-1F / %x21-3F / %x41-FF ) reduces to
// 1*[^\r\n @\0]
static char const* _is_user(char const* peek)
{
  if (!_any_except(*peek, "\r\n @\0", 5)) {
    return NULL;
  }

  while (*peek) {
    if (!_any_except(*peek, "\r\n @\0", 5))
      break;
  }

  return peek;
}

char const* _is_ip4addr(char const* peek)
{
  unsigned i = 3;
  unsigned j = 2;

  while (i-- > 0) {
    if (!isdigit(*peek++))
      return NULL;

    while (j-- > 0) {
      if (!isdigit(*peek))
        break;
      peek++;
    }

    if (*peek == '.')
      peek++;
    j = 2;
  }

  if (!isdigit(*peek++))
    return NULL;

  while (j-- > 0) {
    if (!isdigit(*peek))
      break;
    peek++;
  }

  return peek;
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
