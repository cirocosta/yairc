#ifndef IRC_PARSER__LEXER_H
#define IRC_PARSER__LEXER_H

#include "yairc/common.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define YI_MAXBUFSIZE 512 + 1

typedef enum yi_token_type_e {
  YI_T_LETTER = 1,
  YI_T_DIGIT,
  YI_T_CRLF,
  YI_T_SPACE,
  YI_T_TERMINAL,
  YI_T_SINGLE_TERMINAL,
  YI_T_SHORTNAME,
  YI_T_HOSTNAME
} yi_token_type_e;

typedef struct yi_token_t {
  char buf[YI_MAXBUFSIZE];
  yi_token_type_e type;
  unsigned len;
} yi_token_t;

typedef struct yi_buffer_t {
  char const* la;
  const char* buf;
  unsigned pos;
  size_t size;
  yi_token_t* token;
} yi_buffer_t;

yi_buffer_t* yi_buffer_create(const char* str, size_t buf_len);
void yi_buffer_destroy(yi_buffer_t* buf);
void yi_buffer_init(yi_buffer_t* buf, const char* str, size_t buf_len);
void yi_buffer_destroy(yi_buffer_t* buf);

static inline int yi_in_range(char value, char begin, char end)
{
  return value >= begin && value < end;
}

int yi_lex(yi_buffer_t* buf);
int yi_lex_shortname(yi_buffer_t* buf);
int yi_lex_hostname(yi_buffer_t* buf);
int yi_lex_space(yi_buffer_t* buf);
int yi_lex_crlf(yi_buffer_t* buf);
int yi_lex_letter(yi_buffer_t* buf);
int yi_lex_single_terminal(yi_buffer_t* buf, char c);
int yi_lex_terminal(yi_buffer_t* buf, char* terminal, unsigned size);
int yi_lex_digit(yi_buffer_t* buf);

#endif
