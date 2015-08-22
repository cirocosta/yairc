#ifndef YI__COMMON_H
#define YI__COMMON_H

#include "yairc/debug.h"

#define FREE(ptr)                                                              \
  do {                                                                         \
    free(ptr);                                                                 \
    ptr = 0;                                                                   \
  } while (0)

#define FREE_ARR(ptr, count)                                                   \
  do {                                                                         \
    unsigned i = 0;                                                            \
    while (i < count)                                                          \
      free(ptr[i++]);                                                          \
    free(ptr);                                                                 \
    ptr = 0;                                                                   \
  } while (0)

#endif // ! YI__COMMON_H
