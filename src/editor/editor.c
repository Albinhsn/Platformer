#include "arena.h"
#include "asset.h"
#include "common.h"
#include "editor_ui.h"
#include "entity.h"
#include "font.h"
#include "input.h"
#include "json.h"
#include "renderer.h"

void saveMap(UI* ui)
{
  Arena arena;
  initArena(&arena, 4096);
  Json json;
  json.headType = JSON_OBJECT;
  initJsonObject(&arena, &json.obj);
  JsonObject* head            = &json.obj;

  String      widthKey        = (String){.len = 5, .buffer = (char*)"width", .capacity = 5};
  String      backgroundKey   = (String){.len = 10, .buffer = (char*)"background", .capacity = 10};
  String      heightKey       = (String){.len = 6, .buffer = (char*)"height", .capacity = 6};

  JsonValue   widthValue      = (JsonValue){.type = JSON_NUMBER, .number = ui->map.tileWidth};
  JsonValue   heightValue     = (JsonValue){.type = JSON_NUMBER, .number = ui->map.tileHeight};
  JsonValue   backgroundValue = (JsonValue){.type = JSON_NUMBER, .number = ui->map.backgroundIdx};

  addElementToJsonObject(head, widthKey, widthValue);
  addElementToJsonObject(head, heightKey, heightValue);
  addElementToJsonObject(head, backgroundKey, backgroundValue);

  JsonValue tileArrayValue;
  tileArrayValue.type = JSON_ARRAY;
  initJsonArray(&arena, &tileArrayValue.arr);

  for (u32 i = 0; i < ui->map.tileCount; i++)
  {
    EditorTile tile       = ui->map.tiles[i];
    String     xKey       = (String){.len = 1, .buffer = (char*)"x", .capacity = 1};
    String     yKey       = (String){.len = 1, .buffer = (char*)"y", .capacity = 1};
    String     tileIdxKey = (String){.len = 10, .buffer = (char*)"textureIdx", .capacity = 10};

    JsonValue  xValue     = (JsonValue){.type = JSON_NUMBER, .number = tile.x};
    JsonValue  yValue     = (JsonValue){.type = JSON_NUMBER, .number = tile.y};

    JsonValue tileIdxValue = (JsonValue){.type = JSON_NUMBER, .number = tile.tileIdx};

    JsonValue tileValue;
    tileValue.type = JSON_OBJECT;
    initJsonObject(&arena, &tileValue.obj);

    addElementToJsonObject(&tileValue.obj, xKey, xValue);
    addElementToJsonObject(&tileValue.obj, yKey, yValue);
    addElementToJsonObject(&tileValue.obj, tileIdxKey, tileIdxValue);

    addElementToJsonArray(&tileArrayValue.arr, tileValue);
  }
  String tileArrayKey = (String){.len = 5, .buffer = (char*)"tiles", .capacity = 5};
  addElementToJsonObject(head, tileArrayKey, tileArrayValue);

  serializeToFile(&json, "map.json");
  printf("Saved file!\n");
}

i32 main(int argv, char* argc[])
{
  initGlobalEntities();
  loadStateVariables();
  loadAssetData();

  Font font;
  initRenderer(&font, "./Assets/variables/editorTextureLocation.txt");
  font.textureId = getTextureId(TEXTURE_FONT);

  InputState inputState;

  UI         ui;
  initUI(&ui);
  initTiledTextures();

  Timer timer;
  resetTimer(&timer);
  startTimer(&timer);

  while (true)
  {
    initNewFrame(WHITE);
    if (handleInput(&inputState) || inputState.keyboardStateRelease[ASCII_ESCAPE])
    {
      break;
    }
    updateTimer(&timer);

    if (renderUI(&ui, &inputState, &timer))
    {
      saveMap(&ui);
    }

    SDL_GL_SwapWindow(g_renderer.window);
  }
}
