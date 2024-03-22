#ifndef ENTITY_H
#define ENTITY_H
#include "animation.h"
#include "common.h"
#include "input.h"
#include "timer.h"

enum EntityType
{
  ENTITY_TYPE_DUMB   = 0,
  ENTITY_TYPE_CLOUD  = 1,
  ENTITY_TYPE_ITEM   = 2,
  ENTITY_TYPE_LEVER  = 3,
  ENTITY_TYPE_BUTTON = 4,
  ENTITY_TYPE_ENEMY  = 5,
  ENTITY_TYPE_SRPING = 6,
  ENTITY_TYPE_SPIKES = 7
};
typedef enum EntityType EntityType;

struct Entity
{
  f32 x;
  f32 y;
  f32 width;
  f32 height;
  union
  {
    Animation* animation;
    u32        textureIdx;
  };
  bool animated;
};
typedef struct Entity Entity;

enum TileType
{
  TILE_TYPE_GROUND = 0,
  TILE_TYPE_MIDDLE = 1,
  TILE_TYPE_SPAWN  = 2,
  TILE_TYPE_END    = 3
};
typedef enum TileType TileType;

struct Enemy
{
  f32 xAcc;
  f32 yAcc;
  f32 maxX;
  f32 maxY;
};
typedef struct Enemy Enemy;

struct Cloud
{
  Entity* entities;
  f32     xAcc;
  f32     yAcc;
  f32     maxX;
  f32     maxY;
};
typedef struct Cloud Cloud;

enum ItemType
{
  EMERALD,
  KEY
};
typedef enum ItemType ItemType;

struct Item
{
  ItemType type;
};
typedef struct Item Item;

struct Button
{
  bool pressed;
};
typedef struct Button Button;

struct Lever
{
  u8 state;
};
typedef struct Lever Lever;

struct Spring
{
  u64 lastPressed;
  u64 cd;
};
typedef struct Spring Spring;

struct Player
{
  Entity* entity;
  u64     hp;
  Item*   items;
  f32     yAcc;
  f32     xAcc;
};

typedef struct Player Player;

struct Spike
{
  u64 lastUsed;
  u64 cooldown;
};
typedef struct Spike Spike;

struct Tile
{
  Entity* entity;
  union
  {
    Cloud*  cloud;
    Item*   item;
    Lever*  lever;
    Button* button;
    Enemy*  enemy;
    Spring* spring;
    Spike* spike;
  };
  TileType   type;
  EntityType entityType;
};
typedef struct Tile Tile;

struct Map
{
  Tile* tiles;
  i8    backgroundIdx;
  u8    tileCount;
  u8    width;
  u8    height;
};
typedef struct Map Map;

struct EditorTile
{
  u8 x;
  u8 y;
  u8 tileIdx;
};
typedef struct EditorTile EditorTile;

#define MAX_ENTITY_COUNT 512
extern Entity g_entities[MAX_ENTITY_COUNT];

Entity*       getPlayerEntity();
Entity*       getNewEntity();

void          initGlobalEntities();
void          initEntity(Entity* entity, f32 x, f32 y, f32 width, f32 height, u64 textureIdx, f32 movementSpeed, bool animated);
void          initPlayer(Player* player);

bool          entitiesCollided(Entity* e1, Entity* e2);

void          updatePlayer(InputState* inputState, Player* player, Timer* timer, Map* map);

void          debugPlayer(Player* player);
void          debugEntity(Entity* entity);

#endif
