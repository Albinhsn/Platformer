#ifndef STA_STRING_H
#define STA_STRING_H

#include "common.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct String
{
  u64 len;
  u64 capacity;
  u8* buffer;
};
typedef struct String String;

#endif
