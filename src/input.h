#ifndef INPUT_H
#define INPUT_H

#include "common.h"
#include "string.h"
#include <stdbool.h>

#define INPUT_STATE_LENGTH 127

struct InputState
{
  i32  mouseX;
  i32  mouseY;
  i32  mouseXRel;
  i32  mouseYRel;
  bool md_1;
  bool mr_1;
  bool md_3;
  bool mr_3;
  bool keyboardStateRelease[INPUT_STATE_LENGTH];
  bool keyboardStateDown[INPUT_STATE_LENGTH];
};
typedef struct InputState InputState;

bool                      handleInput(struct InputState* inputState);
void                      resetInputState(InputState* inputState);
void                      debugInputState(InputState* inputState);
inline void               initInputState(struct InputState* inputState)
{
  for (i8 i = 0; i < INPUT_STATE_LENGTH; i++)
  {
    inputState->keyboardStateDown[i] = false;
  }
}
void getKeyboardInputCharacters(struct InputState* inputState, struct String* string);

#endif
