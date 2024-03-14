#ifndef MAP_H
#define MAP_H

#include "common.h"

struct MapTile
{
  u8 textureIdx;
  u8 x;
  u8 y;
};
typedef struct MapTile MapTile;

struct Map
{
  MapTile* tiles;
  i8       backgroundIdx;
  u8       tileCount;
  u8       width;
  u8       height;
};
typedef struct Map Map;

void parseMap(Map * map, u64 mapIdx);
#endif
