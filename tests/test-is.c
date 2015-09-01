#include <string.h>
#include <stdio.h>

#include "yairc/common.h"
#include "yairc/_is.h"

void test1()
{
  const char* msg = "levin.mozilla.org";

  ASSERT(_is_middle(msg), "");
}

void test2()
{
  const char * msg = "a_kw_31-=/31";
  ASSERT(_is_nospcrlfcl(msg), "");
}

void test3()
{
  const char* msg = ":levin.mozilla.org";

  ASSERT(_is_trailing(msg), "");
}

int main(int argc, char *argv[])
{
  TEST(test1, "_is_middle");
  TEST(test2, "_is_nospcrlfcl");
  TEST(test3, "_is_trailing");
  
  return 0;
}
