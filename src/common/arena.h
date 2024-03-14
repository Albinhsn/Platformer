#ifndef ARENA_H
#define ARENA_H
#include "common.h"
#include <stdlib.h>

struct Arena
{
  u64 memory;
  u64   ptr;
  u64   maxSize;
};
typedef struct Arena Arena;
u64 ArenaPush(Arena* arena, u64 size);
void  ArenaPop(Arena* arena, u64 size);
#define ArenaPushArray(arena, type, count) (type*)ArenaPush((arena), sizeof(type) * (count))
#define ArenaPushStruct(arena, type)       ArenaPushArray((arena), type, 1)

inline void initArena(Arena * arena, u64 size){
  arena->ptr = 0;
  arena->maxSize = size;
  arena->memory = (u64)malloc(size);
}

#endif
