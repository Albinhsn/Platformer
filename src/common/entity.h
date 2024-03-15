#ifndef ENTITY_H
#define ENTITY_H
#include "common.h"
#include "input.h"
#include "map.h"
#include "timer.h"

struct Entity
{
  f32 x;
  f32 y;
  f32 width;
  f32 height;
  u32 textureIdx;
  f32 movementSpeed;
};
typedef struct Entity Entity;

struct Player
{
  Entity* entity;
  f32     yAcc;
};
typedef struct Player Player;

struct Enemy
{
  Entity* entity;
};
typedef struct Enemy Enemy;

#define MAX_ENTITY_COUNT 256
extern Entity g_entities[MAX_ENTITY_COUNT];

Entity*       getPlayerEntity();
Entity*       getNewEntity();

void          parseEnemiesFromJson(Json* json, Enemy** enemies);
void          initEntity(Entity* entity, f32 x, f32 y, f32 width, f32 height, u32 textureIdx, f32 movementSpeed);
void          initPlayer(Player* player);

bool          entitiesCollided(Entity* e1, Entity* e2);

void          updatePlayer(InputState* inputState, Player* player, Timer* timer, Map* map);

void          debugPlayer(Player* player);
void          debugEntity(Entity* entity);

#endif
