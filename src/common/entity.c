#include "entity.h"
#include "common.h"
#include "json.h"
#include <stdbool.h>
#include <stdio.h>

Entity      g_entities[256];

static bool withinScreen(Entity* entity)
{
  i32 x = (i32)(100.0f - entity->width / 2.0f);
  i32 y = (i32)(100.0f - entity->height / 2.0f);
  return !(entity->x <= -x || entity->x >= x || entity->y <= -y || entity->y >= y);
}

void parseEnemiesFromJson(Json* json, Map* map, Enemy** enemies_, u64* enemyCount)
{
  JsonObject head      = json->obj;
  JsonArray  array     = (lookupJsonElement(&head, "enemies"))->arr;
  u8         mapWidth  = map->width;
  u8         mapHeight = map->height;

  Enemy*     enemies   = (Enemy*)malloc(sizeof(Enemy) * array.arraySize);
  *enemyCount          = array.arraySize;

  for (u32 i = 0; i < array.arraySize; i++)
  {
    JsonObject tileObj         = array.values[i].obj;
    u8         x               = (lookupJsonElement(&tileObj, "x"))->number;
    u8         y               = (lookupJsonElement(&tileObj, "y"))->number;

    enemies[i].entity          = getNewEntity();
    Entity* enemyEntity        = enemies[i].entity;
    enemyEntity->x             = ((x / (f32)mapWidth) * 2.0f - 1.0f) * 100.0f;
    enemyEntity->y             = -((((f32)y - 0.5f) / (f32)mapHeight) * 2.0f - 1.0f) * 100.0f;
    enemyEntity->height        = (1 / (f32)mapHeight) * 200.0f;
    enemyEntity->width         = (1 / (f32)mapWidth) * 200.0f;
    enemyEntity->movementSpeed = 0.0f;
  }
  *enemies_ = enemies;
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
    Tile tile     = map->tiles[i];
    f32     x        = ((tile.x / (f32)maxWidth) * 2.0f - 1.0f) * 100.0f;
    f32     y        = -((tile.y / (f32)maxHeight) * 2.0f - 1.0f) * 100.0f;

    f32     minTileX = x - width;
    f32     maxTileX = x + width;

    f32     minTileY = y - height;
    f32     maxTileY = y + height;
    bool    ground   = tile.type == TILE_TYPE_GROUND;

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
    Tile tile      = map->tiles[i];

    bool    withinX   = !(minX > tile.x + width || maxX < tile.x - width);
    bool    groundedY = maxY > tile.y + height && maxY - (tile.y + height) <= 1.0f;
    bool    ground    = tile.type == TILE_TYPE_GROUND;


    if (withinX && groundedY && ground)
    {
      return true;
    }
  }
  return false;
}

void updatePlayer(InputState* inputState, Player* player, Timer* timer, Map* map)
{

  f32 ms   = player->entity->movementSpeed;
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
    xAcc += ms;
  }
  if (inputState->keyboardStateDown['a'])
  {
    xAcc -= ms;
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
  entity->movementSpeed = movementSpeed;
  entity->animated      = animated;
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
