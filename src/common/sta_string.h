#ifndef STA_STRING_H
#define STA_STRING_H

#include <stdint.h>

typedef uint64_t u64;

struct String
{
  u64   len;
  u64   capacity;
  char* buffer;
};
typedef struct String String;

#endif
