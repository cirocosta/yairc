#ifndef YI__PARSER_H
#define YI__PARSER_H

#include "yairc/lexer.h"

#define YI_MAX_MESSAGE_PARAMS 15

typedef struct yi_message_t {
  yi_buffer_t* buf;

  char prefix[YI_MAX_MESSAGE_SIZE];
  char command[YI_MAX_MESSAGE_SIZE];
  char parameters[YI_MAX_MESSAGE_PARAMS][YI_MAX_MESSAGE_SIZE]; // TODO wow!
  unsigned char parameters_count;
} yi_message_t;

yi_message_t* yi_message_create(const char* msg, unsigned msg_size);
void yi_message_reset(yi_message_t* message, const char* msg,
                      unsigned msg_size);
void yi_message_destroy(yi_message_t* message);

yi_message_t* yi_parse(const char* src, unsigned src_size);
void yi_parse_m(yi_message_t* message, const char* src, unsigned src_size);

#endif
