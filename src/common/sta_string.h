#ifndef STA_STRING_H
#define STA_STRING_H

#include <stdbool.h>
#include <stdint.h>

typedef uint64_t u64;
typedef int      i32;

struct String
{
  u64   len;
  u64   capacity;
  char* buffer;
};
typedef struct String String;

i32                   sta_compareString(String s1, String s2);

#endif
