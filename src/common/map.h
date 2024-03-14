#ifndef MAP_H
#define MAP_H

#include "common.h"

enum TileType
{
  TILE_TYPE_GROUND,
  TILE_TYPE_MIDDLE,
  TILE_TYPE_SPAWN,
  TILE_TYPE_END
};
typedef enum TileType TileType;

struct MapTile
{
  TileType type;
  u8       textureIdx;
  u8       x;
  u8       y;
};
typedef struct MapTile MapTile;

struct Map
{
  MapTile* tiles;
  i8       backgroundIdx;
  u8       tileCount;
  u8       width;
  u8       height;
  f32      spawnX;
  f32      spawnY;
  f32      endX;
  f32      endY;
};
typedef struct Map Map;

void               parseMap(Map* map, u64 mapIdx);
#endif
