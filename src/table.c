#include "yairc/table.h"

yi_table_t* yi_table_create(size_t size)
{
  yi_table_t* table = malloc(sizeof(*table));
  PASSERT(table, "Couldn't allocate memory");

  *table = YI_TABLE_T_INITIALIZER;
  hcreate_r(size, &table->htab);
  table->size = size;
  table->data = calloc(size, sizeof(*table->data));
  PASSERT(table->data, "Couldn't allocate memory");

  return table;
}

void yi_table_destroy(yi_table_t* table)
{
  hdestroy_r(&table->htab);
  FREE(table->data);
  FREE(table);
}

// TODO if ENTER and there's no more room, ENOMEM would be returned
//      and we could then realloc the table with dobule the size.
int yi_table_add(yi_table_t* table, char* key, void* data)
{
  unsigned n = 0;
  ENTRY e, *ep;

  table->data[table->length] = data;
  table->length++;

  e.key = key;
  e.data = data;
  n = hsearch_r(e, ENTER, &ep, &table->htab);

  if (!n)
    return 0;

  return n;
}

void* yi_table_get(yi_table_t* table, char* key)
{
  unsigned n = 0;
  ENTRY e, *ep;

  e.key = key;
  n = hsearch_r(e, FIND, &ep, &table->htab);

  if (!n)
    return NULL;

  return ep->data;
}
