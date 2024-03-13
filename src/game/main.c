#include "common.h"
#include "entity.h"
#include "game_renderer.h"
#include "game_ui.h"
#include "input.h"
#include "renderer.h"
#include "timer.h"
#include <stdlib.h>
#include <string.h>

struct Game
{
  Timer  timer;
  u64    lastUpdated;
  Player player;
};

typedef struct Game Game;

static void         resetGame(Game* game)
{
  resetTimer(&game->timer);
  printf("Restarted game\n");
}

static void gameLoop(UIState* state, InputState* inputState, Game* game)
{
  updateTimer(&game->timer);
  if (shouldHandleUpdates(&game->timer, &game->lastUpdated))
  {
    if (handleInput(inputState))
    {
      *state = UI_EXIT;
    }
    updatePlayer(inputState, &game->player, &game->timer);
  }
  renderEntity(game->player.entity);
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

i32 main()
{
  srand(0);
  loadStateVariables();

  Font font;
  initRenderer(&font, "./Assets/variables/gameTextureLocation.txt");
  font.textureId = getTextureId(TEXTURE_FONT);


  InputState inputState;

  UI         ui;
  ui.state = UI_MAIN_MENU;
  ConsoleUI      console;
  GameOverUI     gameOver;
  MainMenuUI     mainMenu;
  PauseMenuUI    pauseMenu;
  SettingsMenuUI settingsMenu;
  initUI(&ui, &console, &gameOver, &mainMenu, &pauseMenu, &settingsMenu);

  u64  prevTick = 0;

  Game game;
  memset(&game, 0, sizeof(Game));
  resetGame(&game);

  game.player.hp     = 3;
  game.player.entity = getNewEntity();
  initEntity(game.player.entity, 0.0f, 0.0f, 10.0f, 10.0f, 0, 1.0f);

  while (ui.state != UI_EXIT)
  {

    initNewFrame(BLACK);
    renderInfoStrings(&prevTick);

    if (ui.state == UI_GAME_RUNNING)
    {
      if (!game.timer.running)
      {
        startTimer(&game.timer);
      }
      gameLoop(&ui.state, &inputState, &game);
    }
    else if (handleInput(&inputState))
    {
      break;
    }

    UIState newState = renderUI(&ui, &inputState);
    updateUIState(&ui, newState, &game.timer);

    SDL_GL_SwapWindow(g_renderer.window);
  }

  SDL_Quit();
  return 0;
}
