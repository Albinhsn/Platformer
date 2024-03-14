#include "map.h"
#include "arena.h"
#include "files.h"
#include "json.h"

static const char* mapFileLocations[] = {"./Assets/Maps/map01.json"};

void               parseMap(Map* map, u64 mapIdx)
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

  map->backgroundIdx = (lookupJsonElement(&json.obj, "background"))->number;
  map->width     = (lookupJsonElement(&json.obj, "width"))->number;
  map->height    = (lookupJsonElement(&json.obj, "height"))->number;
  JsonArray array    = (lookupJsonElement(&json.obj, "tiles"))->arr;

  map->tileCount     = array.arraySize;
  map->tiles         = (MapTile*)malloc(sizeof(MapTile) * array.arraySize);
  for (u32 i = 0; i < array.arraySize; i++)
  {
    JsonObject tile          = array.values[i].obj;
    map->tiles[i].x          = (lookupJsonElement(&tile, "x"))->number;
    map->tiles[i].y          = (lookupJsonElement(&tile, "y"))->number;
    map->tiles[i].textureIdx = (lookupJsonElement(&tile, "textureIdx"))->number;
  }
}
