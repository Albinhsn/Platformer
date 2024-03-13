#include "editor_ui.h"
#include "editor_renderer.h"
#include "renderer.h"
#include "vector.h"
#include <stdlib.h>

void initUIMap(UIMap* map, f32 w, f32 h)
{
  map->width     = w;
  map->height    = h;
  map->tileCount = 0;
  map->tileCap   = 8;
  map->tiles     = (UIMapTile*)malloc(sizeof(UIMapTile) * map->tileCap);
}
void initUI(UI* ui)
{
  initUIMap(&ui->map, 100.0f, 100.0f);
}

void renderUI(UI* ui)
{
  // SELECTABLE TILES
  TextureTiled tiles      = getTiledTexture(TEXTURE_TILES);
  u32          totalTiles = (tiles.texture->width * tiles.texture->height) / (tiles.dim * tiles.dim);
  f32          start      = 42.0f;
  f32          dim        = 4.0f;
  f32          x          = start;
  f32          y          = 96.0f;
  for (u32 i = 0; i < totalTiles; i++)
  {
    renderTextureTile(x, y, dim, dim, TEXTURE_TILES, i);
    x += dim * 2.0f;
    if (x >= 100.f)
    {
      x = start;
      y -= dim * 2.0f;
    }
  }

  renderUnfilledQuad(CREATE_VEC2f32(-90.0f, 60.0f), CREATE_VEC2f32(30.0f, -60.0f), 5, &RED);
}
