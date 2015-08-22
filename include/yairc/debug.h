#ifndef YI__DEBUG_H
#define YI__DEBUG_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#define __MAXLINE 4096

#define LOG(message, ...)                                                      \
  do {                                                                         \
    fprintf(stdout, message, ##__VA_ARGS__);                                   \
  } while (0)

#define LOGERR(message, ...)                                                   \
  do {                                                                         \
    fprintf(stderr, message, ##__VA_ARGS__);                                   \
  } while (0)

#define PASSERT(condition, message, ...)                                       \
  do {                                                                         \
    if (!(condition)) {                                                        \
      fprintf(stderr, message, ##__VA_ARGS__);                                 \
      fprintf(stderr, "%s\n", strerror(errno));                                \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
  } while (0)

#define ASSERT(condition, message, ...)                                        \
  do {                                                                         \
    if (!(condition)) {                                                        \
      fprintf(stderr, "\n" __BASE_FILE__ " : %2d\n", __LINE__);                \
      fprintf(stderr, "Assertion `%s` failed\n", #condition);                  \
      fprintf(stderr, "\t" message "\n", ##__VA_ARGS__);                       \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
  } while (0)

#ifndef NDEBUG
#define DASSERT(condition, message, ...)                                       \
  ASSERT(condition, message, ##__VA_ARGS__)
#define DLOG(message, ...) LOG(message, ##__VA_ARGS__)
#define DLOGERR(message, ...) LOGERR(message, ##__VA_ARGS__)
#define DPASSERT(condition, message, ...)                                      \
  PASSERT(condition, message, ##__VA_ARGS__)
#else
#define DASSERT(condition, message, ...)
#define DLOG(message, ...)
#define DLOGERR(message, ...)
#define DASSERT(condition, message, ...)
#endif // ! NDEBUG

#endif // ! YI__DEBUG_H
