#include "entity.h"
#include "input.h"
#include "timer.h"
#include <endian.h>
#include <stdbool.h>
#include <stdio.h>

Entity      g_entities[256];

static bool withinScreen(Entity* entity)
{
  i32 x = (i32)(100.0f - entity->width / 2.0f);
  i32 y = (i32)(100.0f - entity->height / 2.0f);
  return !(entity->x <= -x || entity->x >= x || entity->y <= -y || entity->y >= y);
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
    if (g_entities[i].textureIdx == 0)
    {
      return &g_entities[i];
    }
  }
  return 0;
}

void updatePlayer(InputState* inputState, Player* player, Timer* timer)
{

  f32 xAcc = 0.0f, yAcc = 0.0f;
  f32 ms = player->entity->movementSpeed;

  if (inputState->keyboardStateDown['w'])
  {
    yAcc += ms;
  }
  if (inputState->keyboardStateDown['s'])
  {
    yAcc -= ms;
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
  entity->y += yAcc;
  if (!withinScreen(entity))
  {
    entity->x -= xAcc;
    entity->y -= yAcc;
  }
}

void initEntity(Entity* entity, f32 x, f32 y, f32 width, f32 height, u32 textureIdx, f32 movementSpeed)
{
  entity->x             = x;
  entity->y             = y;
  entity->width         = width;
  entity->height        = height;
  entity->textureIdx    = textureIdx;
  entity->movementSpeed = movementSpeed;
}

void debugEntity(Entity* entity)
{

  printf("%f %f %f %f %d\n", entity->x, entity->y, entity->width, entity->height, entity->textureIdx);
}


void createPlayer(Player* player)
{
  player->entity  = getPlayerEntity();
  initEntity(player->entity, 0.0f, 0.0f, 5.0f, 10.0f, 1, 1.0f);
}
