#ifndef EDITOR_UI_H
#define EDITOR_UI_H

#include "common.h"


struct UIMapTile
{
  u8 textureIdx;
  u8 x;
  u8 y;
};
typedef struct UIMapTile UIMapTile;

struct UIMap
{
  UIMapTile* tiles;
  u8         tileCap;
  u8         tileCount;
  u8         width;
  u8         height;
};
typedef struct UIMap UIMap;

struct UITiles
{
  f32 x;
  f32 y;
  f32 width;
  f32 height;
  u8  textureIdx;
};
typedef struct UITiles UITiles;

struct UI
{
  UIMap map;
  UITiles tileMap;
};
typedef struct UI UI;

void              renderUI(UI* ui);
void              initUI(UI* ui);

#endif
