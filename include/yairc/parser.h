#ifndef YI__PARSER_H
#define YI__PARSER_H 

typedef struct yi_message_t {
  char* prefix;
  char* command;
  char** parameters;
} yi_message_t;

void yi_parse();

#endif
