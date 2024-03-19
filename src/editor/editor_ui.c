#include "editor_ui.h"
#include "common.h"
#include "entity.h"
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

  for (u32 i = 0; i < map->tileCount; i++)
  {
    EditorTile tile = map->tiles[i];
    renderTextureTile(minX + (tile.x + 0.5f) * dimX * 2, minY - (tile.y + 0.5f) * dimY * 2, dimX, dimY, TEXTURE_TILES, map->tiles[i].tileIdx);
  }
}
static void initUIMap(UIMap* map, f32 x, f32 y, f32 w, f32 h)
{
  initUIComponent(&map->comp, x, y, w, h, -1);
  map->tileWidth     = 20;
  map->tileHeight    = 20;

  map->tileCap       = 8;
  map->tileCount     = 0;
  map->tiles         = (EditorTile*)malloc(sizeof(EditorTile) * map->tileCap);

  map->backgroundIdx = -1;
}

static void initUITiles(UITiles* tiles, f32 x, f32 y, f32 width, f32 height, f32 tileDim)
{
  tiles->tileDim = 4.0f;
  initUIComponent(&tiles->comp, x, y, width, height, -1);

  TileData* tileData = g_tileData;
  tiles->tileCounter = g_tileDataCounter;
  tiles->tiles       = (Tile*)malloc(sizeof(Tile) * tiles->tileCounter);
  printf("Creating %ld entities\n", tiles->tileCounter);
  for (u64 i = 0; i < tiles->tileCounter; i++)
  {
    Tile*     tile   = &tiles->tiles[i];
    TileData* data   = &tileData[i];
    tile->type       = data->tileType;
    tile->entityType = data->entityType;
    tile->entity     = getNewEntity();
    Entity* entity   = tile->entity;
    entity->animated = data->animated;

    if (entity->animated)
    {
      entity->animation = (Animation*)malloc(sizeof(Animation));
      initAnimation(entity->animation, &data->animationData);
    }
    else
    {
      entity->textureIdx = data->textureIdx;
    }
  }
}

static void initUISelected(UIComponent* selected, f32 x, f32 y, f32 w, f32 h)
{
  selected->x          = x;
  selected->y          = y;
  selected->width      = w;
  selected->height     = h;
  selected->textureIdx = 0;
}

void initUI(UI* ui)
{
  initUIMap(&ui->map, -30.0f, 0.0f, 60.0f, 60.0f);
  initUITiles(&ui->tiles, 68.0f, 0.0f, 32.0f, 100.0f, 4.0f);
  initUISelected(&ui->selected, -30.0f, 80.0f, 10.0f, 10.0f);

  initButton(&ui->saveBtn, RED, "save", 5.0f, 10.0f, -10.0f, -80.0f, 15.0f, 15.0f, TEXTURE_GREY_BUTTON_05);
}

static void renderTiles(UITiles* tiles, Timer* timer)
{

  f32 dim       = tiles->tileDim;
  f32 start     = tiles->comp.x - tiles->comp.width + dim;
  f32 x         = start;
  f32 endX      = tiles->comp.x + tiles->comp.width;
  f32 y         = tiles->comp.y + tiles->comp.height - dim;

  f32 doubleDim = dim * 2.0f;
  for (u32 i = 0; i < tiles->tileCounter; i++)
  {
    printf("%ld\n", (u64)tiles);
    Entity* entity = tiles[i].tiles->entity;
    if (entity->animated)
    {
      updateAnimation(entity->animation, timer);
      printf("%ld\n", entity->animation->currentTexture);
      printf("%ld\n", entity->animation->animationData->textureCount);
      u64 textureIdx = entity->animation->animationData->textureIds[entity->animation->currentTexture];
      renderTextureTile(x, y, dim, dim, TEXTURE_TILES, textureIdx);
    }
    else
    {
      printf("%d\n", entity->textureIdx);
      renderTextureTile(x, y, dim, dim, TEXTURE_TILES, entity->textureIdx);
    }
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
  return (tiles.comp.height * tiles.comp.width) / (tiles.tileDim * tiles.tileDim);
}

static void getNewSelectedTile(UI* ui, InputState* inputState)
{
  u32     screenWidth  = getScreenWidth();
  u32     screenHeight = getScreenHeight();
  f32     mouseX       = inputState->mouseX / (f32)screenWidth * 200.0f - 100.0f;
  f32     mouseY       = inputState->mouseY / (f32)screenHeight * 200.0f - 100.0f;

  UITiles tiles        = ui->tiles;

  f32     x            = (mouseX - tiles.comp.x + tiles.comp.width) / (tiles.comp.width * 2);
  f32     y            = (mouseY - tiles.comp.y + tiles.comp.height) / (tiles.comp.height * 2);

  u32     colIdx       = tiles.comp.width / tiles.tileDim * x;
  u32     rowIdx       = tiles.comp.height / tiles.tileDim * y;

  u32     newIdx       = colIdx + rowIdx * tiles.comp.width / tiles.tileDim;
  if (newIdx >= getNumberOfTiles(tiles))
  {
    return;
  }
  ui->selected.textureIdx = newIdx;
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

static void addTileToMap(UIMap* map, InputState* inputState, u32 tileIdx)
{
  u32         screenWidth  = getScreenWidth();
  u32         screenHeight = getScreenHeight();
  f32         mouseX       = inputState->mouseX / (f32)screenWidth * 200.0f - 100.0f;
  f32         mouseY       = inputState->mouseY / (f32)screenHeight * 200.0f - 100.0f;

  f32         x            = (mouseX - map->comp.x + map->comp.width) / (map->comp.width * 2);
  f32         y            = (mouseY - map->comp.y + map->comp.height) / (map->comp.height * 2);

  u32         colIdx       = map->tileWidth * x;
  u32         rowIdx       = map->tileHeight * y;

  EditorTile* tiles        = map->tiles;
  for (u32 i = 0; i < map->tileCount; i++)
  {
    if (tiles[i].x == colIdx && tiles[i].y == rowIdx)
    {
      if (tiles[i].tileIdx != tileIdx)
      {
        tiles[i].tileIdx = tileIdx;
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
    map->tiles = realloc(map->tiles, sizeof(EditorTile) * map->tileCap);
  }

  map->tiles[map->tileCount].x       = colIdx;
  map->tiles[map->tileCount].y       = rowIdx;
  map->tiles[map->tileCount].tileIdx = tileIdx;
  map->tileCount++;
}

bool renderUI(UI* ui, InputState* inputState, Timer* timer)
{
  if (componentIsReleased(ui->tiles.comp, inputState))
  {
    getNewSelectedTile(ui, inputState);
  }
  renderTiles(&ui->tiles, timer);

  if (componentIsReleased(ui->map.comp, inputState))
  {
    addTileToMap(&ui->map, inputState, ui->selected.textureIdx);
  }

  rebindFullTexture();
  renderButton(&ui->saveBtn);

  renderUIMap(&ui->map);
  UIComponent* comp = &ui->selected;
  renderTextureTile(comp->x, comp->y, comp->width, comp->height, TEXTURE_TILES, comp->textureIdx);

  debugTileMap(&ui->map);
  return componentIsReleased(ui->saveBtn.component, inputState);
}
