#ifndef MAP_H
#define MAP_H

#include "animation.h"
#include "common.h"
#include "json.h"

enum TileType
{
  TILE_TYPE_GROUND,
  TILE_TYPE_MIDDLE,
  TILE_TYPE_SPAWN,
  TILE_TYPE_END
};
typedef enum TileType TileType;

struct Tile
{
  TileType type;
  union
  {
    u64        textureIdx;
    Animation animation;
  };
  bool animated;
  f32 x;
  f32 y;
};
typedef struct Tile Tile;

struct Map
{
  Tile* tiles;
  f32   spawnX;
  f32   spawnY;
  f32   endX;
  f32   endY;
  i8    backgroundIdx;
  u8    tileCount;
  u8    width;
  u8    height;
};
typedef struct Map Map;

void               parseMapFromJson(Json* json, Map* map);
#endif
