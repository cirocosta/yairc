#ifndef YI__LEXER_H
#define YI__LEXER_H

#include "yairc/common.h"
#include "yairc/_is.h"

#include <stdlib.h>

#define YI_MAXBUFSIZE 512 + 1

typedef enum yi_token_type_e {
  YI_T_SINGLE_TERMINAL = 1,
  YI_T_TERMINAL,
  YI_T_COMMAND,
  YI_T_PARAM,
  YI_T_PREFIX_HOSTNAME,
  YI_T_PREFIX_NICKNAME,
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
void yi_buffer_reset(yi_buffer_t* buf, const char* str, size_t buf_len);
void yi_buffer_destroy(yi_buffer_t* buf);


int yi_lex_single_terminal(yi_buffer_t* buf, char c);
int yi_lex_terminal(yi_buffer_t* buf, char* terminal, unsigned size);

int yi_lex_command(yi_buffer_t* buf);
int yi_lex_prefix(yi_buffer_t* buf);
int yi_lex_param_middle(yi_buffer_t* buf);
int yi_lex_param_trailing(yi_buffer_t* buf);

#endif
