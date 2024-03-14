#include "map.h"
#include "arena.h"
#include "files.h"
#include "json.h"

static const char* mapFileLocations[] = {"./Assets/Maps/map01.json"};

TileType           parseTileType(u32 textureIdx)
{

  switch (textureIdx)
  {
  case 0:
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

void parseMap(Map* map, u64 mapIdx)
{
  Arena arena;
  initArena(&arena, 4096);
  Json   json;
  String fileContent;
  bool   res = readFile((char**)&fileContent.buffer, (i32*)&fileContent.len, mapFileLocations[mapIdx]);
  if (!res)
  {
    printf("Failed to parse map file\n");
    exit(1);
  }

  deserializeFromString(&arena, &json, fileContent);

  map->backgroundIdx  = (lookupJsonElement(&json.obj, "background"))->number;
  map->width          = (lookupJsonElement(&json.obj, "width"))->number;
  map->height         = (lookupJsonElement(&json.obj, "height"))->number;
  JsonArray array     = (lookupJsonElement(&json.obj, "tiles"))->arr;

  u8        maxWidth  = map->width;
  u8        maxHeight = map->height;

  map->tileCount      = array.arraySize;
  map->tiles          = (MapTile*)malloc(sizeof(MapTile) * array.arraySize);
  for (u32 i = 0; i < array.arraySize; i++)
  {
    JsonObject tileObj = array.values[i].obj;
    MapTile*   tile    = &map->tiles[i];
    tile->x            = (lookupJsonElement(&tileObj, "x"))->number;
    tile->y            = (lookupJsonElement(&tileObj, "y"))->number;
    tile->textureIdx   = (lookupJsonElement(&tileObj, "textureIdx"))->number;
    tile->type         = parseTileType(map->tiles[i].textureIdx);

    if (map->tiles[i].type == TILE_TYPE_SPAWN)
    {
      map->spawnX = ((tile->x / (f32)maxWidth) * 2.0f - 1.0f) * 100.0f;
      map->spawnY = -((tile->y / (f32)maxHeight) * 2.0f - 1.0f) * 100.0f;
    }
    else if (map->tiles[i].type == TILE_TYPE_END)
    {
      map->endX = ((tile->x / (f32)maxWidth) * 2.0f - 1.0f) * 100.0f;
      map->endY = -((tile->y / (f32)maxHeight) * 2.0f - 1.0f) * 100.0f;
    }
  }
}
