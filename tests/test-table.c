#include <stdio.h>

#include "yairc/common.h"
#include "yairc/table.h"

void test1()
{
  yi_table_t* table = yi_table_create(10);
  void* d = NULL;

  yi_table_add(table, "key1", "111");
  yi_table_add(table, "key2", "222");
  yi_table_add(table, "key3", "333");

  ASSERT((d = yi_table_get(table, "key1")) != NULL, "");
  STRNCMP((char*)d, "111");

  ASSERT((d = yi_table_get(table, "key2")) != NULL, "");
  STRNCMP((char*)d, "222");

  ASSERT((d = yi_table_get(table, "LOL")) == NULL, "");

  yi_table_destroy(table);
}

int main(int argc, char* argv[])
{
  TEST(test1);

  return 0;
}

