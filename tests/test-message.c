#include "yairc/connection.h"
#include "yairc/common.h"
#include "yairc/parser.h"

#include <libgen.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>

int counter = 0;
yi_message_t messages[] = {
  {.buf = NULL, .prefix = "\0", .command = "CAP", .parameters = { "LS" } },
  {.buf = NULL,
   .prefix = "\0",
   .command = "NICK",
   .parameters = { "cirocosta" } },
  {.buf = NULL,
   .prefix = "\0",
   .command = "USER",
   .parameters = { "Ciro", "Ciro", "localhost", " realname" } },
};

void message_processer1(void* arg, yi_message_t* message)
{
  STRNCMP(message->prefix, messages[counter].prefix);
  STRNCMP(message->command, messages[counter].command);
  STRNCMP(message->parameters[0], messages[counter].parameters[0]);
  STRNCMP(message->parameters[1], messages[counter].parameters[1]);
  STRNCMP(message->parameters[2], messages[counter].parameters[2]);
  STRNCMP(message->parameters[3], messages[counter].parameters[3]);

  counter++;
}

/**
 * Properly written
 * without extra CRLFs
 */
void test1()
{
  char* path = "./tests/assets/log1.txt";
  FILE* fp = fopen(path, "r");

  counter = 0;

  if (!fp) {
    perror("file open:");
    exit(EXIT_FAILURE);
  }

  yi_message_parse_fd(fileno(fp), NULL, message_processer1);
  fclose(fp);
}

/**
 * With extra CRLFs
 */
void test2()
{
  char* path = "./tests/assets/log2.txt";
  FILE* fp = fopen(path, "r");

  counter = 0;

  if (!fp) {
    perror("file open:");
    exit(EXIT_FAILURE);
  }

  yi_message_parse_fd(fileno(fp), NULL, message_processer1);
  fclose(fp);
}

int main(int argc, char* argv[])
{
  setbuf(stdout, NULL);

  TEST(test1);
  TEST(test2);

  return 0;
}
