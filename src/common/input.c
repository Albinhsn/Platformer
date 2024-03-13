#include "input.h"
#include "common.h"
#include <SDL2/SDL.h>

void getKeyboardInputCharacters(InputState* inputState, String* string)
{
  for (int i = 0; i < INPUT_STATE_LENGTH; i++)
  {
    if (inputState->keyboardStateRelease[i])
    {
      if (i == ASCII_BACKSPACE)
      {
        string->buffer[string->len--] = 0;
      }
      else
      {
        if (string->len == string->capacity)
        {
          string->buffer = (u8*)realloc(string->buffer, string->capacity * 2);
        }
        string->buffer[string->len] = i;
        string->len++;
      }
    }
  }
}
static inline void handleKeyRelease(bool* key, bool* previousDown, bool down)
{
  if (*previousDown == true && !down)
  {
    *key          = true;
    *previousDown = false;
  }
  else
  {
    *previousDown = down;
  }
}

static inline void handleMouseInput(InputState* inputState, bool down, u8 button)
{
  switch (button)
  {
  case SDL_BUTTON_LEFT:
  {
    handleKeyRelease(&inputState->mr_1, &inputState->md_1, down);
    break;
  }
  case SDL_BUTTON_RIGHT:
  {
    handleKeyRelease(&inputState->mr_3, &inputState->md_3, down);
    break;
  }
  default:
  {
    break;
  }
  }
}

static inline void handleKeyboardInput(InputState* inputState, bool down, i32 sym)
{
  if (sym >= INPUT_STATE_LENGTH)
  {
    return;
  }
  if (inputState->keyboardStateDown[sym] == true && !down)
  {
    inputState->keyboardStateRelease[sym] = true;
  }
  inputState->keyboardStateDown[sym] = down;
}

void resetInputState(InputState* inputState)
{
  memset(&inputState->keyboardStateRelease[0], 0, INPUT_STATE_LENGTH);
  inputState->mr_1 = false;
  inputState->mr_3 = false;
}
void debugInputState(InputState* inputState)
{
  printf("w:%d a:%d s:%d d:%d space:%d\n", inputState->keyboardStateRelease['w'], inputState->keyboardStateRelease['a'], inputState->keyboardStateRelease['s'], inputState->keyboardStateRelease['d'],
         inputState->keyboardStateRelease[ASCII_SPACE]);
}

bool handleInput(InputState* inputState)
{
  SDL_Event event;
  bool      mouseEvent = false;
  resetInputState(inputState);

  while (SDL_PollEvent(&event))
  {
    switch (event.type)
    {
    case SDL_QUIT:
    {
      return true;
    }
    case SDL_MOUSEMOTION:
    {
      mouseEvent            = true;
      inputState->mouseX    = event.motion.x;
      inputState->mouseY    = event.motion.y;
      inputState->mouseXRel = event.motion.xrel;
      inputState->mouseYRel = event.motion.yrel;
      break;
    }
    case SDL_MOUSEBUTTONUP:
    {
      handleMouseInput(inputState, false, event.button.button);
      break;
    }
    case SDL_MOUSEBUTTONDOWN:
    {
      handleMouseInput(inputState, true, event.button.button);
      break;
    }
    case SDL_KEYUP:
    {
      handleKeyboardInput(inputState, false, event.key.keysym.sym);
      break;
    }
    case SDL_KEYDOWN:
    {
      handleKeyboardInput(inputState, true, event.key.keysym.sym);
      break;
    }
    default:
    {
      break;
    }
    }
  }
#if DEBUG_INPUT
  for (int i = 0; i < INPUT_STATE_LENGTH; i++)
  {
    printf("%d ", inputState->keyboardState[i]);
  }
  printf("\n");

#endif
  if (!mouseEvent)
  {
    SDL_GetMouseState(&inputState->mouseX, &inputState->mouseY);
  }

  return false;
}
