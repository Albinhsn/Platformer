#include "editor_ui.h"
#include "common.h"
#include "editor_renderer.h"
#include "renderer.h"
#include "ui.h"
#include "vector.h"
#include <stdlib.h>

static void renderUIMap(UIMap* map)
{
  UIComponent comp = map->comp;
  f32         minX = comp.x - comp.width;
  f32         minY = comp.y + comp.height;

  f32         dimX = map->comp.width / map->tileWidth;
  f32         dimY = map->comp.height / map->tileHeight;

  if (map->backgroundIdx != -1)
  {
    renderTextureTile(comp.x, comp.y, comp.width, comp.height, TEXTURE_BACKGROUNDS, map->backgroundIdx);
  }

  // renderUnfilledQuad(CREATE_VEC2f32(minX, minY), CREATE_VEC2f32(maxX, maxY), 5, &RED);
  for (u32 i = 0; i < map->tileCount; i++)
  {
    UIMapTile tile = map->tiles[i];
    renderTextureTile(minX + (tile.col + 0.5f) * dimX * 2, minY - (tile.row + 0.5f) * dimY * 2, dimX, dimY, TEXTURE_TILES, map->tiles[i].textureIdx);
  }

  for (u32 i = 0; i < map->characterCount; i++)
  {
    UIMapTile tile = map->characters[i];
    renderTextureTile(minX + (tile.col + 0.5f) * dimX * 2, minY - (tile.row + 0.5f) * dimY * 2, dimX, dimY, TEXTURE_CHARACTERS, map->tiles[i].textureIdx);
  }
}
static void initUIMap(UIMap* map, f32 x, f32 y, f32 w, f32 h)
{
  initUIComponent(&map->comp, x, y, w, h, -1);
  map->tileWidth      = 20;
  map->tileHeight     = 20;

  map->tileCap        = 8;
  map->tileCount      = 0;
  map->tiles          = (UIMapTile*)malloc(sizeof(UIMapTile) * map->tileCap);

  map->characterCap   = 8;
  map->characterCount = 0;
  map->characters     = (UIMapTile*)malloc(sizeof(UIMapTile) * map->characterCap);

  map->backgroundIdx  = -1;
}

static void initUITiles(UITiles* tiles, TextureTiled* textureTiled, f32 x, f32 y, f32 width, f32 height, f32 tileDim)
{
  tiles->tiles   = textureTiled;
  tiles->tileDim = 4.0f;
  initUIComponent(&tiles->comp, x, y, width, height, -1);
}

static void initUISelected(UIComponent* selected, f32 x, f32 y, f32 w, f32 h)
{
  selected->x          = x;
  selected->y          = y;
  selected->width      = w;
  selected->height     = h;
  selected->textureIdx = 0;
}

static void initUISelectedType(UISelectType* type)
{
  type->models[0] = TEXTURE_TILES;
  type->models[1] = TEXTURE_CHARACTERS;
  type->models[2] = TEXTURE_BACKGROUNDS;

  initUIComponent(&type->types[0], -80.0f, -80.0f, 10.0f, 10.0f, -1);
  initUIComponent(&type->types[1], -60.0f, -80.0f, 10.0f, 10.0f, -1);
  initUIComponent(&type->types[2], -40.0f, -80.0f, 10.0f, 10.0f, -1);
}

void initUI(UI* ui)
{
  initUIMap(&ui->map, -30.0f, 0.0f, 60.0f, 60.0f);
  initUITiles(&ui->tiles, getTiledTexture(TEXTURE_TILES), 68.0f, 0.0f, 32.0f, 100.0f, 4.0f);
  initUISelected(&ui->selected, -30.0f, 80.0f, 10.0f, 10.0f);
  initUISelectedType(&ui->selectTypes);
  ui->selectedTileType = TEXTURE_TILES;
}

static void renderTiles(UITiles* tiles, TextureModel model)
{

  Texture* texture    = tiles->tiles->texture;
  u32      totalTiles = (texture->width * texture->height) / (tiles->tiles->dim * tiles->tiles->dim);

  f32      dim        = tiles->tileDim;
  f32      start      = tiles->comp.x - tiles->comp.width + dim;
  f32      x          = start;
  f32      endX       = tiles->comp.x + tiles->comp.width;
  f32      y          = tiles->comp.y + tiles->comp.height - dim;

  f32      doubleDim  = dim * 2.0f;
  for (u32 i = 0; i < totalTiles; i++)
  {
    renderTextureTile(x, y, dim, dim, model, i);
    x += doubleDim;
    if (x >= endX)
    {
      x = start;
      y -= doubleDim;
    }
  }
}

static inline u32 getNumberOfTiles(UITiles tiles)
{
  return (tiles.tiles->texture->height * tiles.tiles->texture->width) / (tiles.tiles->dim * tiles.tiles->dim);
}

static void getNewSelectedTile(UI* ui, InputState* inputState)
{
  u32     screenWidth  = getScreenWidth();
  u32     screenHeight = getScreenHeight();
  f32     mouseX       = inputState->mouseX / (f32)screenWidth * 200.0f - 100.0f;
  f32     mouseY       = inputState->mouseY / (f32)screenHeight * 200.0f - 100.0f;

  UITiles tiles = ui->tiles;

  f32 x      = (mouseX - tiles.comp.x + tiles.comp.width) / (tiles.comp.width * 2);
  f32 y      = (mouseY - tiles.comp.y + tiles.comp.height) / (tiles.comp.height * 2);

  u32 colIdx = tiles.comp.width / tiles.tileDim * x;
  u32 rowIdx = tiles.comp.height / tiles.tileDim * y;

  u32 newIdx = colIdx + rowIdx * tiles.comp.width / tiles.tileDim;
  if (newIdx >= getNumberOfTiles(tiles))
  {
    return;
  }
  ui->selected.textureIdx = newIdx;
}

static void renderSelectedTile(UIComponent* comp, TextureModel selectedTextureModel)
{
  renderTextureTile(comp->x, comp->y, comp->width, comp->height, selectedTextureModel, comp->textureIdx);
}

static void debugTileMap(UIMap* map)
{
  f32 diffX = map->comp.width * 2.0f / map->tileWidth;
  f32 diffY = map->comp.height * 2.0f / map->tileHeight;

  f32 minX  = map->comp.x - map->comp.width;
  f32 minY  = map->comp.y + map->comp.height;
  for (u32 i = 0; i < map->tileWidth * map->tileHeight; i++)
  {
    f32 maxX = minX + diffX;
    f32 maxY = minY - diffY;

    renderUnfilledQuad(CREATE_VEC2f32(minX, minY), CREATE_VEC2f32(maxX, maxY), 1.0f, &GREEN);
    minX += diffX;
    if (minX >= map->comp.x + map->comp.width)
    {
      minX = map->comp.x - map->comp.width;
      minY -= diffY;
    }
  }
}
static void addCharacterToMap(UIMap* map, InputState* inputState, u32 textureIdx)
{
  u32        screenWidth  = getScreenWidth();
  u32        screenHeight = getScreenHeight();
  f32        mouseX       = inputState->mouseX / (f32)screenWidth * 200.0f - 100.0f;
  f32        mouseY       = inputState->mouseY / (f32)screenHeight * 200.0f - 100.0f;

  f32        x            = (mouseX - map->comp.x + map->comp.width) / (map->comp.width * 2);
  f32        y            = (mouseY - map->comp.y + map->comp.height) / (map->comp.height * 2);

  u32        colIdx       = map->tileWidth * x;
  u32        rowIdx       = map->tileHeight * y;

  UIMapTile* chars        = map->characters;
  for (u32 i = 0; i < map->characterCount; i++)
  {
    if (chars[i].col == colIdx && chars[i].row == rowIdx)
    {
      if (chars[i].textureIdx != textureIdx)
      {
        chars[i].textureIdx = textureIdx;
      }
      else
      {
        for (u32 j = i; j < map->characterCount - 1; j++)
        {
          chars[j] = chars[j + 1];
        }
        map->characterCount--;
      }
      return;
    }
  }
  if (map->characterCount >= map->characterCap)
  {
    map->characterCap *= 2;
    map->characters = realloc(map->characters, sizeof(UIMapTile) * map->characterCap);
  }

  map->characters[map->characterCount].col        = colIdx;
  map->characters[map->characterCount].row        = rowIdx;
  map->characters[map->characterCount].textureIdx = textureIdx;
  map->characterCount++;
}

static void addTileToMap(UIMap* map, InputState* inputState, u32 textureIdx)
{
  u32        screenWidth  = getScreenWidth();
  u32        screenHeight = getScreenHeight();
  f32        mouseX       = inputState->mouseX / (f32)screenWidth * 200.0f - 100.0f;
  f32        mouseY       = inputState->mouseY / (f32)screenHeight * 200.0f - 100.0f;

  f32        x            = (mouseX - map->comp.x + map->comp.width) / (map->comp.width * 2);
  f32        y            = (mouseY - map->comp.y + map->comp.height) / (map->comp.height * 2);

  u32        colIdx       = map->tileWidth * x;
  u32        rowIdx       = map->tileHeight * y;

  UIMapTile* tiles        = map->tiles;
  for (u32 i = 0; i < map->tileCount; i++)
  {
    if (tiles[i].col == colIdx && tiles[i].row == rowIdx)
    {
      if (tiles[i].textureIdx != textureIdx)
      {
        tiles[i].textureIdx = textureIdx;
      }
      else
      {
        for (u32 j = i; j < map->tileCount - 1; j++)
        {
          tiles[j] = tiles[j + 1];
        }
        map->tileCount--;
      }
      return;
    }
  }
  if (map->tileCount >= map->tileCap)
  {
    map->tileCap *= 2;
    map->tiles = realloc(map->tiles, sizeof(UIMapTile) * map->tileCap);
  }

  map->tiles[map->tileCount].col        = colIdx;
  map->tiles[map->tileCount].row        = rowIdx;
  map->tiles[map->tileCount].textureIdx = textureIdx;
  map->tileCount++;
}

void renderSelectedTypeTile(UISelectType* type)
{
  for (u32 i = 0; i < 3; i++)
  {
    UIComponent comp = type->types[i];
    renderTextureTile(comp.x, comp.y, comp.width, comp.height, type->models[i], 0);
  }
}

void renderUI(UI* ui, InputState* inputState)
{
  if (componentIsReleased(ui->tiles.comp, inputState))
  {
    getNewSelectedTile(ui, inputState);
  }
  renderTiles(&ui->tiles, ui->selectedTileType);

  for (u32 i = 0; i < 3; i++)
  {
    if (componentIsReleased(ui->selectTypes.types[i], inputState))
    {
      ui->selectedTileType = ui->selectTypes.models[i];
      ui->tiles.tiles = getTiledTexture(ui->selectedTileType);
      break;
    }
  }

  renderSelectedTypeTile(&ui->selectTypes);

  if (componentIsReleased(ui->map.comp, inputState))
  {
    switch (ui->selectedTileType)
    {
    case TEXTURE_TILES:
    {
      addTileToMap(&ui->map, inputState, ui->selected.textureIdx);
      break;
    }
    case TEXTURE_CHARACTERS:
    {
      addCharacterToMap(&ui->map, inputState, ui->selected.textureIdx);
      break;
    }
    case TEXTURE_BACKGROUNDS:
    {
      ui->map.backgroundIdx = ui->selected.textureIdx;
    }
    default:
    {
    }
    }
  }

  renderUIMap(&ui->map);
  renderSelectedTile(&ui->selected, ui->selectedTileType);

  debugTileMap(&ui->map);
}
