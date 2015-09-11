#ifndef YI__TABLE_H
#define YI__TABLE_H

#include "yairc/common.h"
#include <search.h>
#include <stdlib.h>

typedef struct yi_table_t {
  void** data;
  struct hsearch_data htab;
  size_t length;
  size_t size;
} yi_table_t;
#define YI_TABLE_T_INITIALIZER                                                 \
  (yi_table_t)                                                                 \
  {                                                                            \
    .htab = (struct hsearch_data){ 0 }, .size = 0, .length = 0, .data = NULL   \
  }

yi_table_t* yi_table_create(size_t size);
void yi_table_destroy(yi_table_t* table);
int yi_table_add(yi_table_t* table, char* key, void* data);
void* yi_table_get(yi_table_t* table, char* key);

#endif
