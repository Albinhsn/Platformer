#include "arena.h"
#include "asset.h"
#include "common.h"
#include "entity.h"
#include "files.h"
#include "game_ui.h"
#include "input.h"
#include "json.h"
#include "renderer.h"
#include "timer.h"
#include <stdlib.h>
#include <string.h>

struct Game
{
  Timer  timer;
  u64    lastUpdated;
  u64    mapIdx;
  Player player;
  Map    map;
  Enemy* enemies;
  u64    enemyCount;
};

typedef struct Game Game;

static void         resetPlayer(Player* player, Map* map)
{
  player->yAcc = 0.0f;
  if (player->entity)
  {
    player->entity->x = 0.0f;
    player->entity->y = 0.0f;
  }
}

static void resetGame(Game* game)
{
  resetPlayer(&game->player, &game->map);
  game->lastUpdated = 0;
  resetTimer(&game->timer);
  setStateVariable("reset", 0.0f);
  printf("Restarted game\n");
}

static void checkEndLevel(UIState* state, Game* game)
{
  Map*   map       = &game->map;
  Player player    = game->player;

  u8     maxWidth  = map->width;
  u8     maxHeight = map->height;
  f32    width     = (1.0f / (f32)maxWidth) * 100.0f;
  f32    height    = (1.0f / (f32)maxHeight) * 100.0f;

  for (u32 i = 0; i < game->map.tileCount; i++)
  {
    Tile*   tile       = &map->tiles[i];
    Entity* tileEntity = map->tiles[i].entity;

    if (map->tiles[i].type == TILE_TYPE_END)
    {

      f32 minE1X = tileEntity->x - width;
      f32 maxE1X = tileEntity->x + width;
      f32 minE1Y = tileEntity->y - height;
      f32 maxE1Y = tileEntity->y + height;

      f32 minE2X = player.entity->x - player.entity->width;
      f32 maxE2X = player.entity->x + player.entity->width;
      f32 minE2Y = player.entity->y - player.entity->height;
      f32 maxE2Y = player.entity->y + player.entity->height;

      if (!(minE1X > maxE2X || maxE1X < minE2X) && !(minE1Y > maxE2Y) && !(maxE1Y < minE2Y))
      {
        *state = UI_GAME_OVER;
      }
    }
  }
}

static void gameLoop(UIState* state, InputState* inputState, Game* game)
{
  updateTimer(&game->timer);
  // if (shouldHandleUpdates(&game->timer, &game->lastUpdated))
  // {
  if (handleInput(inputState))
  {
    *state = UI_EXIT;
  }
  //   updatePlayer(inputState, &game->player, &game->timer, &game->map);
  //   checkEndLevel(state, game);
  // }
  renderMap(&game->map, &game->timer);
  // renderEntity(game->player.entity);
}

static void renderInfoStrings(u64* prevTick)
{

  f32  fontSize  = FONT_FONT_SIZE_SMALL;
  f32  spaceSize = FONT_SPACE_SIZE_SMALL;
  f32  x         = -100.0f;
  f32  y         = 60.0f;
  u64  ms        = SDL_GetTicks() - *prevTick;

  char msString[32];
  memset(msString, 0, 32);
  sprintf(msString, "ms:%ld", ms);
  char fpsString[32];
  memset(fpsString, 0, 32);
  sprintf(fpsString, "fps:%d", (u32)MIN(1000.0f / ms, 999));

  renderTextStartsAt(fpsString, &WHITE, x, y - 20.0f, fontSize, spaceSize);
  renderTextStartsAt(msString, &WHITE, x, y, fontSize, spaceSize);

  *prevTick = SDL_GetTicks();
}

static const char* mapFileLocations[] = {"./Assets/Maps/test_map03.json"};

void               parseMap(Game* game)
{
  Arena arena;
  initArena(&arena, 4096);
  Json   json;
  String fileContent;
  bool   res = readFile((char**)&fileContent.buffer, (i32*)&fileContent.len, mapFileLocations[game->mapIdx]);
  if (!res)
  {
    printf("Failed to parse map file\n");
    exit(1);
  }
  deserializeFromString(&arena, &json, fileContent);
  parseTilesFromJson(&json, &game->map);
}

void initGame(Game* game)
{
  memset(game, 0, sizeof(Game));
  resetGame(game);

  game->mapIdx = 0;
  parseMap(game);

  game->player.entity = getNewEntity();
  game->player.yAcc   = 0;
  initEntity(game->player.entity, 0.0f, 0.0f, 5.0f, 5.0f, 0, 0.5f, false);
}

void clearGlobalEntites()
{
  for (u32 i = 0; i < MAX_ENTITY_COUNT; i++)
  {
    g_entities[i].textureIdx = -1;
  }
}

i32 main(int argc, char* argv[])
{
  srand(0);
  loadStateVariables();
  loadAssetData();
  clearGlobalEntites();

  Font font;
  initRenderer(&font, "./Assets/variables/gameTextureLocation.txt");
  font.textureId = getTextureId(TEXTURE_FONT);

  InputState inputState;
  initInputState(&inputState);

  UI ui;
  ui.state = UI_MAIN_MENU;
  ConsoleUI      console;
  GameOverUI     gameOver;
  MainMenuUI     mainMenu;
  PauseMenuUI    pauseMenu;
  SettingsMenuUI settingsMenu;
  initUI(&ui, &console, &gameOver, &mainMenu, &pauseMenu, &settingsMenu);

  u64  prevTick = 0;

  Game game;
  initGame(&game);

  while (ui.state != UI_EXIT)
  {

    initNewFrame(BLACK);

    // if (inputState.keyboardStateRelease['c'])
    // {
    //   updateUIState(&ui, UI_CONSOLE, &game.timer);
    // }

    // if (ui.state == UI_GAME_RUNNING)
    // {
    //   if (getStateVariable("reset") == 1)
    //   {
    //     resetGame(&game);
    //   }
    gameLoop(&ui.state, &inputState, &game);
    // }
    // else if (handleInput(&inputState))
    // {
    //   break;
    // }

    // UIState newState = renderUI(&ui, &inputState);
    // updateUIState(&ui, newState, &game.timer);
    // renderInfoStrings(&prevTick);

    SDL_GL_SwapWindow(g_renderer.window);
  }

  SDL_Quit();
  return 0;
}
