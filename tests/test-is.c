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

void test4()
{
  unsigned i = 4;
  char* msg[] = { "123.123.123.12", "255.255.255.255", "127.0.0.1",
                  "10.10.10.10" };
  while (i-- > 0) {
    ASSERT(_is_ip4addr(msg[i]), "%s is an ip4addr", msg[i]);
  }
}

void test5()
{
  unsigned i = 2;
  char* msg[] = { "FE80:0000:0000:0000:0202:B3FF:FE1E:8329", 
                  "2001:05c0:1000:000b:0000:0000:0000:5c24"
  };
  while (i-- > 0) {
    ASSERT(_is_ip6addr(msg[i]), "%s is an ip6addr", msg[i]);
  }
}

int main(int argc, char *argv[])
{
  TEST(test1, "_is_middle");
  TEST(test2, "_is_nospcrlfcl");
  TEST(test3, "_is_trailing");
  TEST(test4, "_is_trailing");
  TEST(test5, "_is_trailing");
  
  return 0;
}
