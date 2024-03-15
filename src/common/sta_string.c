#include "sta_string.h"
#include <string.h>

bool sta_compareString(String s1, String s2)
{
  u64 minLen = s1.len < s2.len ? s1.len : s2.len;
  return s1.len == s2.len && strncmp(s1.buffer, s2.buffer, minLen) == 0;
}
