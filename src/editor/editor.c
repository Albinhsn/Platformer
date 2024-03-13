#include "common.h"
#include "editor_ui.h"
#include "font.h"
#include "input.h"
#include "renderer.h"

i32 main()
{
  Font font;
  initRenderer(&font);

  InputState inputState;

  UI         ui;
  initUI(&ui);

  while (true)
  {
    initNewFrame(WHITE);
    if (handleInput(&inputState) || inputState.keyboardStateRelease[ASCII_ESCAPE])
    {
      break;
    }

    renderUI(&ui);

    SDL_GL_SwapWindow(g_renderer.window);
  }
}
