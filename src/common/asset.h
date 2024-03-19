#ifndef ASSET_H
#define ASSET_H

#include "animation.h"
#include "common.h"
#include "entity.h"
#include "json.h"

struct TileData
{
  f32    boundsWidth;
  f32    boundsHeight;
  f32    entityType;
  f32    tileType;
  String name;
  union
  {
    AnimationData animationData;
    u32           textureIdx;
  };
  bool animated;
};
typedef struct TileData TileData;

extern StateVariable    g_tileMapMapping[256];
extern u64              g_tileMapMappingCounter;
extern TileData         g_tileData[256];
extern u64              g_tileDataCounter;

f32 getTileMappingValue(String key);
void                    loadAssetData();
TileData*               getTileData(u64 index);

#endif
