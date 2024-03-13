#include "editor_ui.h"
#include "renderer.h"
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

  renderTextCentered("Hello World!", &RED, 0.0f, 0.0f, 10.0f, 10.0f);
}
