#include "asset.h"
#include "entity.h"
#include "json.h"
#include "sta_string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

StateVariable    g_tileMapMapping[256];
u64              g_tileMapMappingCounter = 0;

TileData         g_tileData[256];
u64              g_tileDataCounter = 0;

static TileData* getTileDataByKey(String key)
{

  for (u64 i = 0; i < g_tileDataCounter; i++)
  {
    if (sta_compareString(key, g_tileData[i].name) == 0)
    {
      return &g_tileData[i];
    }
  }
  printf("WARNING: No tileData found with %.*s\n", (i32)key.len, key.buffer);
  return 0;
}

f32 getTileMappingValue(String key)
{
  for (int i = 0; i < g_tileMapMappingCounter; i++)
  {
    u64 minLen = MIN(key.len, strlen(g_tileMapMapping[i].key));
    if (strncmp((char*)key.buffer, g_tileMapMapping[i].key, minLen) == 0)
    {
      return g_tileMapMapping[i].value;
    }
  }
  printf("WARNING: Didn't find tileMapping '%.*s'\n", (i32)key.len, key.buffer);
  return 0.0f;
}

void getTileMappingKey(String* key, u64 value)
{
  for (int i = 0; i < g_tileMapMappingCounter; i++)
  {
    if (g_tileMapMapping[i].value == value)
    {
      key->buffer = g_tileMapMapping[i].key;
      key->len    = strlen(key->buffer);
      return;
    }
  }
  printf("WARNING: Didn't find tileMapping key with '%d'\n", (i32)value);
}

void loadTileMapping()
{
  const char* fileLocation = "./Assets/variables/tiles_map2.txt";
  FILE*       filePtr      = fopen(fileLocation, "rb");
  if (filePtr == 0)
  {
    printf("Failed to open tile maps '%s'\n", fileLocation);
    return;
  }
  char buffer[256];

  while (fgets(buffer, 256, filePtr))
  {
    u8 idx = 0;
    while (buffer[idx] != ' ')
    {
      idx++;
    }

    g_tileMapMapping[g_tileMapMappingCounter].key = (char*)malloc(sizeof(char) * (idx + 1));
    memset(g_tileMapMapping[g_tileMapMappingCounter].key, 0, idx + 1);
    memcpy(g_tileMapMapping[g_tileMapMappingCounter].key, buffer, idx);

    while (buffer[idx] == ' ')
    {
      idx++;
    }

    g_tileMapMapping[g_tileMapMappingCounter].value = atof(&buffer[idx]);
    printf("Added tileMapping %s:%f\n", g_tileMapMapping[g_tileMapMappingCounter].key, g_tileMapMapping[g_tileMapMappingCounter].value);
    g_tileMapMappingCounter++;
  }
}

void setTileMapping(const char* key, f32 value)
{
  for (int i = 0; i < g_tileMapMappingCounter; i++)
  {
    u64 minLen = MIN(strlen(key), strlen(g_tileMapMapping[i].key));
    if (strncmp(key, g_tileMapMapping[i].key, minLen) == 0)
    {
      g_tileMapMapping[i].value = value;
    }
  }
  g_tileMapMapping[g_tileMapMappingCounter++] = (StateVariable){.key = (char*)key, .value = value};
  printf("INFO: Setting tilemapping '%s' as %lf\n", key, value);
}

void loadTileData()
{
  const char* tileLocation = "./Assets/variables/tiles_05.json";
  String      fileString   = {.buffer = (char*)tileLocation, .len = strlen(tileLocation), .capacity = 0};
  Json        tileJson;
  Arena       arena;
  initArena(&arena, 2000);

  deserializeFromFile(&arena, &tileJson, fileString);
  JsonValue* tileValues = tileJson.obj.values;
  String*    tileKeys   = tileJson.obj.keys;
  for (u64 i = 0; i < tileJson.obj.size; i++)
  {
    TileData* data     = &g_tileData[g_tileDataCounter++];
    data->name         = tileKeys[i];

    JsonObject tileObj = tileValues[i].obj;
    data->tileType     = lookupJsonElement(&tileObj, "tileType")->number;
    data->entityType   = lookupJsonElement(&tileObj, "entityType")->number;
    if (data->entityType == ENTITY_TYPE_ITEM)
    {
      data->itemData.itemType = lookupJsonElement(&tileObj, "itemType")->number;
    }

    JsonValue* animation = lookupJsonElement(&tileObj, "animation");
    if (animation)
    {
      JsonObject animationObj          = animation->obj;
      JsonValue* timerValue            = lookupJsonElement(&animationObj, "timer");
      JsonArray  textureKeys           = lookupJsonElement(&animationObj, "textures")->arr;
      data->animated                   = true;
      data->animationData.timer        = timerValue->number;
      data->animationData.textureCount = textureKeys.arraySize;
      data->animationData.textureIds   = (u64*)malloc(sizeof(u64) * textureKeys.arraySize);
      for (u64 i = 0; i < textureKeys.arraySize; i++)
      {
        u64 idx                           = getTileMappingValue(textureKeys.values[i].string);
        data->animationData.textureIds[i] = idx;
      }
    }
    else
    {
      data->animated   = false;
      JsonValue* value = lookupJsonElement(&tileObj, "textureIdx");
      data->textureIdx = value->number;
    }
  }
}

void parseTilesFromJson(Json* json, Map* map)
{
  JsonObject head     = json->obj;
  map->width          = (lookupJsonElement(&head, "width"))->number;
  map->height         = (lookupJsonElement(&head, "height"))->number;

  JsonArray array     = (lookupJsonElement(&head, "tiles"))->arr;
  u8        mapWidth  = map->width;
  u8        mapHeight = map->height;

  map->tiles          = (Tile*)malloc(sizeof(Tile) * array.arraySize);
  map->tileCount      = array.arraySize;

  for (u32 i = 0; i < array.arraySize; i++)
  {
    JsonObject tileObj   = array.values[i].obj;
    u8         x         = (lookupJsonElement(&tileObj, "x"))->number;
    u8         y         = (lookupJsonElement(&tileObj, "y"))->number;

    Tile*      tile      = &map->tiles[i];
    tile->entity         = getNewEntity();

    Entity* tileEntity   = tile->entity;
    tileEntity->x        = ((x / (f32)mapWidth) * 2.0f - 1.0f) * 100.0f;
    tileEntity->y        = -((((f32)y - 0.5f) / (f32)mapHeight) * 2.0f - 1.0f) * 100.0f;
    tileEntity->height   = (1 / (f32)mapHeight) * 100.0f;
    tileEntity->width    = (1 / (f32)mapWidth) * 100.0f;

    u64       textureIdx = (lookupJsonElement(&tileObj, "textureIdx"))->number;

    TileData* tileData   = getTileData(textureIdx);
    if (tileData == 0)
    {
      printf("SEVERE: Failed to get tile data from %ld\n", textureIdx);
      exit(1);
    }

    tileEntity->animated = tileData->animated;
    if (tileEntity->animated)
    {
      tileEntity->animation = (Animation*)malloc(sizeof(Animation));
      initAnimation(tileEntity->animation, &tileData->animationData);
    }
    else
    {
      tileEntity->textureIdx = tileData->textureIdx;
    }

    tile->entityType = tileData->entityType;
    tile->type       = tileData->tileType;
    switch (tile->entityType)
    {
    case ENTITY_TYPE_VERT:
    {
      break;
    }
    case ENTITY_TYPE_DUMB:
    {
      tile->cloud = 0;
      break;
    }
    case ENTITY_TYPE_ITEM:
    {
      tile->item           = (Item*)malloc(sizeof(Item));
      tile->item->type     = tileData->itemData.itemType;
      tile->item->pickedUp = false;
      break;
    }
    case ENTITY_TYPE_CLOUD:
    {
      tile->cloud              = (Cloud*)malloc(sizeof(Cloud));
      tile->cloud->minX        = tile->entity->x - tile->entity->width * 3;
      tile->cloud->maxX        = tile->entity->x + tile->entity->width * 3;
      tile->cloud->xAcc        = 0.1f;
      tile->cloud->maxY        = tile->entity->y;
      tile->cloud->minY        = tile->entity->y;
      tile->cloud->yAcc        = 0;
      tile->cloud->lastUpdated = 0;
      break;
    }
    case ENTITY_TYPE_LEVER:
    {
      tile->lever                              = (Lever*)malloc(sizeof(Lever));
      tile->lever->cd                          = 50;
      tile->lever->lastUpdated                 = 0;
      tile->entity->animation->shouldBeUpdated = false;
      break;
    }
    case ENTITY_TYPE_BUTTON:
    {
      tile->button = (Button*)malloc(sizeof(Button));
      break;
    }
    case ENTITY_TYPE_ENEMY:
    {
      tile->enemy = (Enemy*)malloc(sizeof(Enemy));
      break;
    }
    case ENTITY_TYPE_SPRING:
    {
      tile->spring              = (Spring*)malloc(sizeof(Spring));
      tile->spring->cd          = 500;
      tile->spring->lastPressed = 0;
      break;
    }
    case ENTITY_TYPE_SPIKES:
    {
      tile->spike           = (Spike*)malloc(sizeof(Spike));
      tile->spike->lastUsed = 0;
      tile->spike->cooldown = 100;
      break;
    }
    }
  }
}

TileData* getTileData(u64 index)
{
  return &g_tileData[index];
}

void loadAssetData()
{
  loadTileMapping();
  loadTileData();
}
