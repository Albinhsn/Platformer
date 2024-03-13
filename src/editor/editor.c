#include "common.h"
#include "editor_renderer.h"
#include "editor_ui.h"
#include "font.h"
#include "input.h"
#include "renderer.h"

i32 main()
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
    renderTextureTile(0.0f, 0.0f, 200.0f, 200.0f, 1, 2);

    // renderUI(&ui);

    SDL_GL_SwapWindow(g_renderer.window);
  }
}
