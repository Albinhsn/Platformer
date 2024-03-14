#ifndef EDITOR_UI_H
#define EDITOR_UI_H

#include "common.h"
#include "renderer.h"
#include "texture.h"
#include "ui.h"



struct UIMap
{
  MapTile*  tiles;
  MapTile*  characters;
  UIComponent comp;
  i8          backgroundIdx;
  u8          tileCap;
  u8          tileCount;
  u8          tileWidth;
  u8          tileHeight;
  u8          characterCount;
  u8          characterCap;
};
typedef struct UIMap UIMap;

struct UITiles
{
  TextureTiled* tiles;
  UIComponent   comp;
  f32           tileDim;
};
typedef struct UITiles UITiles;

struct UISelectType
{
  UIComponent  types[3];
  TextureModel models[3];
};
typedef struct UISelectType UISelectType;

struct UI
{
  TextureModel      selectedTileType;
  UIMap             map;
  UITiles           tiles;
  UIComponent       selected;
  UISelectType      selectTypes;
  ButtonUIComponent saveBtn;
};
typedef struct UI UI;

bool              renderUI(UI* ui, InputState* inputState);
void              initUI(UI* ui);

#endif
