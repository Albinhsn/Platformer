#include "entity.h"
#include "animation.h"
#include "common.h"
#include "json.h"
#include "sta_string.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

Entity      g_entities[256];
TileData    g_tileData[256];
u64         g_tileDataCounter = 0;

static bool withinScreen(Entity* entity)
{
  i32 x = (i32)(100.0f - entity->width / 2.0f);
  i32 y = (i32)(100.0f - entity->height / 2.0f);
  return !(entity->x <= -x || entity->x >= x || entity->y <= -y || entity->y >= y);
}

void loadTileData()
{
  const char* tileLocation = "./Assets/variables/tiles_01.json";
  String      fileString   = {.buffer = (char*)tileLocation, .len = strlen(tileLocation), .capacity = 0};
  Json        tileJson;
  Arena       arena;
  initArena(&arena, 2000);

  deserializeFromFile(&arena, &tileJson, fileString);
  JsonValue* tileValues = tileJson.obj.values;
  String*    tileKeys   = tileJson.obj.keys;
  for (u64 i = 0; i < tileJson.obj.size; i++)
  {
    TileData* data       = &g_tileData[g_tileDataCounter++];
    data->name           = tileKeys[i];

    JsonObject tileObj   = tileValues[i].obj;

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
        data->animationData.textureIds[i] = getTileMappingValue(textureKeys.values[i].string);
      }
    }
  }
}

static TileData* getTileData(String key)
{

  for (u64 i = 0; i < g_tileDataCounter; i++)
  {
    if (sta_compareString(key, g_tileData[i].name))
    {
      return &g_tileData[i];
    }
  }
  printf("WARNING: No tileData found with %.*s\n", (i32)key.len, key.buffer);
  return 0;
}

void parseTilesFromJson(Json* json, Map* map)
{
  JsonObject head      = json->obj;
  JsonArray  array     = (lookupJsonElement(&head, "enemies"))->arr;
  u8         mapWidth  = map->width;
  u8         mapHeight = map->height;

  map->tiles           = (Tile*)malloc(sizeof(Tile) * array.arraySize);
  map->tileCount       = array.arraySize;

  for (u32 i = 0; i < array.arraySize; i++)
  {
    JsonObject tileObj = array.values[i].obj;
    u8         x       = (lookupJsonElement(&tileObj, "x"))->number;
    u8         y       = (lookupJsonElement(&tileObj, "y"))->number;

    Tile*      tile    = &map->tiles[i];
    tile->entity       = getNewEntity();

    Entity* tileEntity = tile->entity;
    tileEntity->x      = ((x / (f32)mapWidth) * 2.0f - 1.0f) * 100.0f;
    tileEntity->y      = -((((f32)y - 0.5f) / (f32)mapHeight) * 2.0f - 1.0f) * 100.0f;
    tileEntity->height = (1 / (f32)mapHeight) * 200.0f;
    tileEntity->width  = (1 / (f32)mapWidth) * 200.0f;

    u64    textureIdx  = (lookupJsonElement(&tileObj, "textureIdx"))->number;

    String key         = (String){.len = 0, .buffer = 0, .capacity = 0};
    getTileMappingKey(&key, textureIdx);
    if (key.buffer != 0)
    {
      TileData* tileData   = getTileData(key);
      tileEntity->animated = tileData->animated;
      if (tileEntity->animated)
      {
        tileEntity->animation = (Animation*)malloc(sizeof(Animation));
        initAnimation(tileEntity->animation, &tileData->animationData);
      }
      else
      {
        tileEntity->textureIdx = tileEntity->textureIdx;
      }
      tile->entityType = tileData->entityType;
      tile->type       = tileData->tileType;
      switch (tile->entityType)
      {
      case ENTITY_TYPE_DUMB:
      {
        tile->cloud = 0;
        break;
      }
      case ENTITY_TYPE_ITEM:
      {
        tile->item = (Item*)malloc(sizeof(Item));
        break;
      }
      case ENTITY_TYPE_CLOUD:
      {
        tile->cloud = (Cloud*)malloc(sizeof(Cloud));
        break;
      }
      case ENTITY_TYPE_LEVER:
      {
        tile->lever = (Lever*)malloc(sizeof(Lever));
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
      case ENTITY_TYPE_SRPING:
      {
        tile->spring = (Spring*)malloc(sizeof(Spring));
        break;
      }
      }
    }
  }
}

bool entitiesCollided(Entity* e1, Entity* e2)
{
  if (!withinScreen(e1) || !withinScreen(e2))
  {
    return false;
  }
  f32 minE1X = e1->x - e1->width;
  f32 maxE1X = e1->x + e1->width;
  f32 minE1Y = e1->y - e1->height;
  f32 maxE1Y = e1->y + e1->height;

  f32 minE2X = e2->x - e2->width;
  f32 maxE2X = e2->x + e2->width;
  f32 minE2Y = e2->y - e2->height;
  f32 maxE2Y = e2->y + e2->height;

  if (minE1X > maxE2X || maxE1X < minE2X)
  {
    return false;
  }
  return !(minE1Y > maxE2Y) && !(maxE1Y < minE2Y);
}

Entity* getPlayerEntity()
{
  return &g_entities[0];
}

Entity* getNewEntity()
{
  for (i32 i = 1; i < MAX_ENTITY_COUNT; i++)
  {
    if (g_entities[i].textureIdx == -1)
    {
      return &g_entities[i];
    }
  }
  printf("FAILED TO GET NEW ENTITY\n");
  return 0;
}

static bool collided(Player* player, Map* map)
{
  f32 minX      = player->entity->x - player->entity->width;
  f32 maxX      = player->entity->x + player->entity->width;

  f32 minY      = player->entity->y - player->entity->height;
  f32 maxY      = player->entity->y + player->entity->height;

  u8  tileCount = map->tileCount;

  u8  maxWidth  = map->width;
  u8  maxHeight = map->height;

  f32 width     = (1.0f / (f32)maxWidth) * 100.0f;
  f32 height    = (1.0f / (f32)maxHeight) * 100.0f;

  for (u8 i = 0; i < tileCount; i++)
  {
    Entity* tileEntity = map->tiles[i].entity;
    f32     x          = ((tileEntity->x / (f32)maxWidth) * 2.0f - 1.0f) * 100.0f;
    f32     y          = -((tileEntity->y / (f32)maxHeight) * 2.0f - 1.0f) * 100.0f;

    f32     minTileX   = x - width;
    f32     maxTileX   = x + width;

    f32     minTileY   = y - height;
    f32     maxTileY   = y + height;
    bool    ground     = map->tiles[i].type == TILE_TYPE_GROUND;

    if (ground && !(minX > maxTileX || maxX < minTileX) && !(minY > maxTileY) && !(maxY < minTileY))
    {
      return true;
    }
  }
  return false;
}
#define MIN_COLLISION_AMOUNT 0.5f;
#define MAX_GRAVITY          -1.0f
#define GRAVITY_DELTA        0.5f

static bool isGrounded(Player* player, Map* map)
{
  if (player->yAcc > 0)
  {
    return false;
  }

  f32 minX      = player->entity->x - player->entity->width;
  f32 maxX      = player->entity->x + player->entity->width;

  f32 maxY      = player->entity->y - player->entity->height;

  u8  tileCount = map->tileCount;

  u8  maxWidth  = map->width;
  u8  maxHeight = map->height;

  f32 width     = (1.0f / (f32)maxWidth) * 100.0f;
  f32 height    = (1.0f / (f32)maxHeight) * 100.0f;

  for (u8 i = 0; i < tileCount; i++)
  {
    Entity* tileEntity = map->tiles[i].entity;

    bool    withinX    = !(minX > tileEntity->x + width || maxX < tileEntity->x - width);
    bool    groundedY  = maxY > tileEntity->y + height && maxY - (tileEntity->y + height) <= 1.0f;
    bool    ground     = map->tiles[i].type == TILE_TYPE_GROUND;

    if (withinX && groundedY && ground)
    {
      return true;
    }
  }
  return false;
}

void updatePlayer(InputState* inputState, Player* player, Timer* timer, Map* map)
{

  f32 xAcc = 0.0f;

  if (!isGrounded(player, map) || player->yAcc > 0)
  {
    player->yAcc -= GRAVITY_DELTA;
    player->yAcc = player->yAcc < MAX_GRAVITY ? MAX_GRAVITY : player->yAcc;
  }
  else
  {
    player->yAcc = inputState->keyboardStateDown['w'] ? getStateVariable("jump") : 0;
  }

  if (inputState->keyboardStateDown['d'])
  {
    xAcc += 1.0f;
  }
  if (inputState->keyboardStateDown['a'])
  {
    xAcc -= 1.0f;
  }

  Entity* entity = player->entity;

  entity->x += xAcc;
  if (!withinScreen(entity) || collided(player, map))
  {
    entity->x -= xAcc;
  }

  entity->y += player->yAcc;
  if (!withinScreen(entity))
  {
    entity->y -= player->yAcc;
  }
}

void initEntity(Entity* entity, f32 x, f32 y, f32 width, f32 height, u64 textureIdx, f32 movementSpeed, bool animated)
{
  entity->x      = x;
  entity->y      = y;
  entity->width  = width;
  entity->height = height;
  if (entity->animated)
  {
    entity->animation = (Animation*)textureIdx;
  }
  else
  {
    entity->textureIdx = textureIdx;
  }
  entity->animated = animated;
}

void debugEntity(Entity* entity)
{

  printf("%f %f %f %f %d\n", entity->x, entity->y, entity->width, entity->height, entity->textureIdx);
}

void createPlayer(Player* player)
{
  player->entity = getPlayerEntity();
  initEntity(player->entity, 0.0f, 0.0f, 5.0f, 10.0f, 1, 1.0f, false);
}
