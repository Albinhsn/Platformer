#include "sta_string.h"
#include <string.h>

i32 sta_compareString(String s1, String s2)
{
  u64 minLen = s1.len < s2.len ? s1.len : s2.len;
  i32 cmp    = strncmp(s1.buffer, s2.buffer, minLen);
  if (cmp != 0)
  {
    return cmp;
  }
  if (s1.len == s2.len)
  {
    return 0;
  }
  if (s1.len > s2.len)
  {
    return 1;
  }
  return -1;
}
