#ifndef YI__PARSER_H
#define YI__PARSER_H

#include "yairc/common.h"
#include "yairc/lexer.h"

typedef enum yi_message_status_e {
  YI_MESSAGE_OK = 1,
  YI_MESSAGE_MALFORMED,
  YI_MESSAGE_NUL
} yi_message_status_e;

typedef struct yi_message_t {
  yi_buffer_t* buf;

  char prefix[YI_MAX_MESSAGE];
  char command[YI_MAX_MESSAGE];
  char parameters[YI_MAX_MESSAGE_PARAMS][YI_MAX_MESSAGE]; // TODO wow!
  unsigned char parameters_count;
} yi_message_t;

yi_message_t* yi_message_create(const char* msg, unsigned msg_size);
void yi_message_reset(yi_message_t* message, const char* msg,
                      unsigned msg_size);
void yi_message_destroy(yi_message_t* message);

yi_message_status_e yi_parse_m(yi_message_t* message, const char* src,
                               unsigned src_size);

#endif
