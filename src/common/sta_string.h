#ifndef STA_STRING_H
#define STA_STRING_H

#include <stdint.h>
#include <stdbool.h>

typedef uint64_t u64;

struct String
{
  u64   len;
  u64   capacity;
  char* buffer;
};
typedef struct String String;

bool                  sta_compareString(String s1, String s2);

#endif
