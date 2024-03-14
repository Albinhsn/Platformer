#include "arena.h"
#include "common.h"
#include "editor_ui.h"
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

  String      widthKey        = (String){.len = 5, .buffer = (u8*)"width", .capacity = 5};
  String      backgroundKey   = (String){.len = 10, .buffer = (u8*)"background", .capacity = 10};
  String      heightKey       = (String){.len = 6, .buffer = (u8*)"height", .capacity = 6};

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
    MapTile   tile            = ui->map.tiles[i];
    String    xKey            = (String){.len = 1, .buffer = (u8*)"x", .capacity = 1};
    String    yKey            = (String){.len = 1, .buffer = (u8*)"y", .capacity = 1};
    String    textureIdxKey   = (String){.len = 10, .buffer = (u8*)"textureIdx", .capacity = 10};

    JsonValue xValue          = (JsonValue){.type = JSON_NUMBER, .number = tile.x};
    JsonValue yValue          = (JsonValue){.type = JSON_NUMBER, .number = tile.y};
    JsonValue textureIdxValue = (JsonValue){.type = JSON_NUMBER, .number = tile.textureIdx};

    JsonValue tileValue;
    tileValue.type = JSON_OBJECT;
    initJsonObject(&arena, &tileValue.obj);

    addElementToJsonObject(&tileValue.obj, xKey, xValue);
    addElementToJsonObject(&tileValue.obj, yKey, yValue);
    addElementToJsonObject(&tileValue.obj, textureIdxKey, textureIdxValue);

    addElementToJsonArray(&tileArrayValue.arr, tileValue);
  }
  String tileArrayKey = (String){.len = 5, .buffer = (u8*)"tiles", .capacity = 5};
  addElementToJsonObject(head, tileArrayKey, tileArrayValue);

  JsonValue enemyArrayValue;
  initJsonArray(&arena, &enemyArrayValue.arr);
  enemyArrayValue.type = JSON_ARRAY;

  for (u32 i = 0; i < ui->map.characterCount; i++)
  {
    MapTile   tile            = ui->map.characters[i];
    String    xKey            = (String){.len = 1, .buffer = (u8*)"x", .capacity = 1};
    String    yKey            = (String){.len = 1, .buffer = (u8*)"y", .capacity = 1};
    String    textureIdxKey   = (String){.len = 10, .buffer = (u8*)"textureIdx", .capacity = 10};

    JsonValue xValue          = (JsonValue){.type = JSON_NUMBER, .number = tile.x};
    JsonValue yValue          = (JsonValue){.type = JSON_NUMBER, .number = tile.y};
    JsonValue textureIdxValue = (JsonValue){.type = JSON_NUMBER, .number = tile.textureIdx};

    JsonValue tileValue;
    tileValue.type = JSON_OBJECT;
    initJsonObject(&arena, &tileValue.obj);

    addElementToJsonObject(&tileValue.obj, xKey, xValue);
    addElementToJsonObject(&tileValue.obj, yKey, yValue);
    addElementToJsonObject(&tileValue.obj, textureIdxKey, textureIdxValue);

    addElementToJsonArray(&enemyArrayValue.arr, tileValue);
  }
  String enemyArrayKey = (String){.len = 7, .buffer = (u8*)"enemies", .capacity = 7};
  addElementToJsonObject(head, enemyArrayKey, enemyArrayValue);

  serializeToFile(&json, "map.json");
  printf("Saved file!\n");
}

i32 main(int argv, char* argc[])
{
  loadStateVariables();

  Font font;
  initRenderer(&font, "./Assets/variables/editorTextureLocation.txt");
  font.textureId = getTextureId(TEXTURE_FONT);

  InputState inputState;

  UI         ui;
  initUI(&ui);
  initTiledTextures();

  while (true)
  {
    initNewFrame(WHITE);
    if (handleInput(&inputState) || inputState.keyboardStateRelease[ASCII_ESCAPE])
    {
      break;
    }

    if (renderUI(&ui, &inputState))
    {
      saveMap(&ui);
    }

    SDL_GL_SwapWindow(g_renderer.window);
  }
}
