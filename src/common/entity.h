#ifndef ENTITY_H
#define ENTITY_H
#include "animation.h"
#include "common.h"
#include "input.h"
#include "json.h"
#include "timer.h"

enum EntityType
{
  ENTITY_TYPE_DUMB,
  ENTITY_TYPE_CLOUD,
  ENTITY_TYPE_ITEM,
  ENTITY_TYPE_LEVER,
  ENTITY_TYPE_BUTTON,
  ENTITY_TYPE_ENEMY,
  ENTITY_TYPE_SRPING
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
  Item*   items;
  f32     yAcc;
  f32     xAcc;
};

typedef struct Player Player;

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

#define MAX_ENTITY_COUNT 256
extern Entity g_entities[MAX_ENTITY_COUNT];

Entity*       getPlayerEntity();
Entity*       getNewEntity();

void          parseTilesFromJson(Json* json, Map* map);
void          initEntity(Entity* entity, f32 x, f32 y, f32 width, f32 height, u64 textureIdx, f32 movementSpeed, bool animated);
void          initPlayer(Player* player);
void          loadTileData();

bool          entitiesCollided(Entity* e1, Entity* e2);

void          updatePlayer(InputState* inputState, Player* player, Timer* timer, Map* map);

void          debugPlayer(Player* player);
void          debugEntity(Entity* entity);

#endif
