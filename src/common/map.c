#include "map.h"
#include "animation.h"
#include "json.h"

TileType parseTileType(u32 textureIdx)
{

  switch (textureIdx)
  {
  case 0:
  {
  }
  case 1:
  {
  }
  case 2:
  {
    return TILE_TYPE_GROUND;
  }
  case 130:
  {
    return TILE_TYPE_END;
  }
  case 150:
  {
    return TILE_TYPE_SPAWN;
  }
  default:
  {
    return TILE_TYPE_MIDDLE;
  }
  }
}

void parseMapFromJson(Json* json, Map* map)
{

  map->backgroundIdx  = (lookupJsonElement(&json->obj, "background"))->number;
  map->width          = (lookupJsonElement(&json->obj, "width"))->number;
  map->height         = (lookupJsonElement(&json->obj, "height"))->number;
  JsonArray array     = (lookupJsonElement(&json->obj, "tiles"))->arr;

  u8        maxWidth  = map->width;
  u8        maxHeight = map->height;

  map->tileCount      = array.arraySize;
  map->tiles          = (Tile*)malloc(sizeof(Tile) * array.arraySize);

  for (u32 i = 0; i < array.arraySize; i++)
  {
    JsonObject tileObj = array.values[i].obj;
    Tile*      tile    = &map->tiles[i];
    tile->x            = lookupJsonElement(&tileObj, "x")->number;
    tile->x            = ((tile->x / (f32)maxWidth) * 2.0f - 1.0f) * 100.0f;

    tile->y            = lookupJsonElement(&tileObj, "y")->number;
    tile->y            = ((tile->y / (f32)maxHeight) * 2.0f - 1.0f) * -100.0f;

    u64 textureIdx     = (lookupJsonElement(&tileObj, "textureIdx"))->number;
    tile->type         = parseTileType(textureIdx);

    String key         = (String){.len = 0, .buffer = 0, .capacity = 0};
    getTileMappingKey(&key, textureIdx);
    AnimationData* data = getAnimationData(key);
    tile->animated      = data != 0;
    if (tile->animated)
    {
      initAnimation(&tile->animation, data);
    }
    else
    {
      tile->textureIdx = textureIdx;
    }

    if (map->tiles[i].type == TILE_TYPE_SPAWN)
    {
      map->spawnX = tile->x;
      map->spawnY = tile->y;
    }
    else if (map->tiles[i].type == TILE_TYPE_END)
    {
      map->endX = tile->x;
      map->endY = tile->y;
    }
  }
}
