#ifndef EDITOR_UI_H
#define EDITOR_UI_H

#include "common.h"
#include "entity.h"
#include "ui.h"

struct UIMap
{
  EditorTile* tiles;
  UIComponent comp;
  i8          backgroundIdx;
  u8          tileCap;
  u8          tileCount;
  u8          tileWidth;
  u8          tileHeight;
};
typedef struct UIMap UIMap;

struct UITiles
{
  Tile*       tiles;
  u64         tileCounter;
  UIComponent comp;
  f32         tileDim;
};
typedef struct UITiles UITiles;

struct UI
{
  UIMap             map;
  UITiles           tiles;
  UIComponent       selected;
  ButtonUIComponent saveBtn;
};
typedef struct UI UI;

bool              renderUI(UI* ui, InputState* inputState, Timer* timer);
void              initUI(UI* ui);

#endif
