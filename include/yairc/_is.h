#ifndef YI___IS_H
#define YI___IS_H

#include "yairc/common.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

inline static int _in_range(char value, char begin, char end)
{
  return value >= begin && value < end;
}

// call it with: _any_except((char[]){v1,v2,...vn}, n);
inline static int _any_except(char val, char* exceptions, unsigned size)
{
  while (size-- > 0)
    if (val == exceptions[size])
      return 0;

  return 1;
}

inline static char const* _is_single_terminal(char const* la, char c)
{
  return *la == c ? ++la : NULL;
}

inline static char const* _is_digit(char const* la)
{
  if (!_in_range(*la, 0x30, 0x39))
    return NULL;

  return la + 1;
}

inline static char const* _is_terminal(char const* peek, char* terminal,
                                       unsigned size)
{
  if (!peek)
    return NULL;

  if (strncmp(peek, terminal, size))
    return NULL;

  return peek + size;
}

inline static char const* _is_shortname(char const* peek)
{
  // alteranation
  if (!(isdigit(*peek) || isalpha(*peek)))
    return NULL;

  peek++;

  // kleene-star
  while (isalpha(*peek) || isdigit(*peek) || *peek == '-')
    peek++;
  while (isalpha(*peek) || isdigit(*peek))
    peek++;

  return peek;
}

inline static char const* _is_hostname(char const* peek)
{
  char const* tmp_peek;

  if (!(peek = _is_shortname(peek)))
    return NULL;

  tmp_peek = peek;

  // kleene-star
  while (*peek) {
    // concatenation
    if ((tmp_peek = _is_single_terminal(tmp_peek, '.')) &&
        (tmp_peek = _is_shortname(tmp_peek))) {
      peek = tmp_peek;
      continue;
    }
    break;
  }

  return peek;
}

// nickname = (letter|special) *8(letter|digit|special|"-")
// (letter|special) --reduce--> [0x41-0x7D]
// so: [0x41-0x7D]*8([0x41-0x7D]|digit|"-")
inline static char const* _is_nickname(char const* peek)
{
  unsigned max_it = 8;

  if (!_in_range(*peek, 0x41, 0x7D))
    return NULL;
  peek++;

  while (max_it-- > 0) {
    if (_in_range(*peek, 0x41, 0x7D) || isdigit(*peek) || *peek == '-')
      peek++;
    else
      break;
  }

  return peek;
}

// 1*( %x01-09 / %x0B-0C / %x0E-1F / %x21-3F / %x41-FF ) reduces to
// 1*[^\r\n @\0]
inline static char const* _is_user(char const* peek)
{
  if (!_any_except(*peek, "\r\n @\0", 5))
    return NULL;

  while (*peek) {
    if (!_any_except(*peek, "\r\n @\0", 5))
      break;
    peek++;
  }

  return peek;
}

// [^\0\r\n :]
inline static char const* _is_nospcrlfcl(char const* peek)
{
  return _any_except(*peek, "\0\r\n :", 5) ? peek + 1 : NULL;
}

// *(":"|" "|nospcrlfcl)
inline static char const* _is_trailing(char const* peek)
{
  while (*peek && (*peek == ':' || *peek == ' ' || _is_nospcrlfcl(peek)))
    peek++;

  return peek;
}

// nospcrlfcl *(":" | nospcrlfcl)
inline static char const* _is_middle(char const* peek)
{
  if (!(peek = _is_nospcrlfcl(peek)))
    return NULL;

  while (*peek && (*peek == ':' || _is_nospcrlfcl(peek)))
    peek++;

  return peek;
}

// 1*letter / 3digit
inline static char const* _is_command(char const* peek)
{
  unsigned i;

  if (isdigit(*peek)) {
    i = 2;
    peek++;

    while (i-- > 0)
      if (!isdigit(*peek++))
        return NULL;
    return peek;
  }

  if (!isalpha(*peek++))
    return NULL;

  while (*peek) {
    if (!isalpha(*peek))
      break;
    peek++;
  }

  return peek;
}

// TODO include the ip6+ip4 addr type
inline static char const* _is_ip6addr(char const* peek)
{
  unsigned i;

  if (!isxdigit(*peek++))
    return NULL;

  while (*peek) {
    if (!isxdigit(*peek))
      break;
    peek++;
  }

  i = 7;

  while (i-- > 0) {
    if (*peek++ != ':')
      return NULL;

    if (!isxdigit(*peek++))
      return NULL;

    while (*peek) {
      if (!isxdigit(*peek))
        break;
      peek++;
    }
  }

  return peek;
}

inline static char const* _is_ip4addr(char const* peek)
{
  unsigned i = 3;
  unsigned j = 2;

  while (i-- > 0) {
    if (!isdigit(*peek++))
      return NULL;

    while (j-- > 0) {
      if (!isdigit(*peek))
        break;
      peek++;
    }

    if (*peek == '.')
      peek++;
    j = 2;
  }

  if (!isdigit(*peek++))
    return NULL;

  while (j-- > 0) {
    if (!isdigit(*peek))
      break;
    peek++;
  }

  return peek;
}

inline static char const* _is_host(char const* peek)
{
  if ((peek = _is_hostname(peek)))
    return peek;
  else if ((peek = _is_ip4addr(peek)))
    return peek;
  else if ((peek = _is_ip6addr(peek)))
    return peek;

  return NULL;
}

inline static char const* _is_prefix_hostname(char const* peek)
{
  if (!(peek = _is_hostname(peek)))
    return NULL;

  if (!(peek = _is_single_terminal(peek, ' ')))
    return NULL;

  return peek;
}

inline static char const* _is_prefix_nickname(char const* la)
{
  char const* peek;
  char const* tmp;

  if (!(peek = _is_nickname(la)))
    return 0;

  tmp = peek;

  if ((tmp = _is_single_terminal(peek, '!'))) {
    if (!(tmp = _is_user(tmp)))
      return NULL;
    if (!(tmp = _is_single_terminal(tmp, '@')))
      return NULL;
    if (!(tmp = _is_host(tmp)))
      return NULL;
    peek = tmp;
  }

  else if ((tmp = _is_single_terminal(peek, '@'))) {
    if (!(tmp = _is_host(tmp)))
      return NULL;
    peek = tmp;
  }

  if (!(peek = _is_single_terminal(peek, ' ')))
    return NULL;

  return peek;
}

#endif
