#include "common.h"
#include "renderer.h"
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct StateVariable
{
  char* key;
  f32   value;
};
typedef struct StateVariable StateVariable;
StateVariable                g_stateVariables[64];
u64                          g_stateVariableCounter = 0;

bool shouldHandleUpdates(Timer* timer, u64* lastUpdated)
{
  u64 lastTick = timer->lastTick;

  if (lastTick >= *lastUpdated + 16)
  {
    *lastUpdated = lastTick;
    return true;
  }
  return false;
}

void                         parseIntFromString(int* dest, char* source, u8* length)
{
  char number[32];
  memset(number, 0, 32);

  for (int i = 0; i < 32; i++)
  {
    number[i] = 0;
  }
  u8 pos = 0;
  while (isdigit(source[pos]))
  {
    pos++;
  }
  memcpy(number, source, pos);
  *dest   = atoi(number);
  *length = pos;
}

void parseFloatFromString(float* dest, char* source, u8* length)
{
  char number[32];
  u8   pos = 0;
  while (source[pos] != ' ')
  {
    pos++;
  }
  memcpy(number, source, pos);
  *dest   = atof(number);
  *length = pos;
}

f32 getRandomFloat(f32 min, f32 max)
{
  return ((f32)rand() / RAND_MAX) * (max - min) + min;
}

void loadStateVariables()
{
  const char* fileLocation = "./Assets/variables/state.txt";
  FILE*       filePtr      = fopen(fileLocation, "rb");
  if (filePtr == 0)
  {
    printf("Failed to open stateVariables '%s'\n", fileLocation);
    return;
  }
  char buffer[256];

  while (fgets(buffer, 256, filePtr))
  {
    u8 idx = 0;
    while (buffer[idx] != ' ')
    {
      idx++;
    }
    g_stateVariables[g_stateVariableCounter].key = (char*)malloc(sizeof(char) * (idx + 1));
    memset(g_stateVariables[g_stateVariableCounter].key, 0, idx + 1);
    memcpy(g_stateVariables[g_stateVariableCounter].key, buffer, idx);

    while (buffer[idx] == ' ')
    {
      idx++;
    }

    g_stateVariables[g_stateVariableCounter].value = atof(&buffer[idx]);
    printf("Added %s:%f\n", g_stateVariables[g_stateVariableCounter].key, g_stateVariables[g_stateVariableCounter].value);
    g_stateVariableCounter++;
  }
}

void setStateVariable(const char* key, f32 value)
{
  for (int i = 0; i < g_stateVariableCounter; i++)
  {
    u64 minLen = MIN(strlen(key), strlen(g_stateVariables[i].key));
    if (strncmp(key, g_stateVariables[i].key, minLen) == 0)
    {
      g_stateVariables[i].value = value;
    }
  }
  g_stateVariables[g_stateVariableCounter++] = (StateVariable){.key = (char*)key, .value = value};
}

f32 getStateVariable(const char* key)
{
  for (int i = 0; i < g_stateVariableCounter; i++)
  {
    u64 minLen = MIN(strlen(key), strlen(g_stateVariables[i].key));
    if (strncmp(key, g_stateVariables[i].key, minLen) == 0)
    {
      return g_stateVariables[i].value;
    }
  }
  printf("WARNING: Didn't find variable '%s'\n", key);
  return 0.0f;
}

f32 convertFloatToBE(f32 f)
{
  f32   retVal;
  char* toConvert   = (char*)&f;
  char* returnFloat = (char*)&retVal;

  returnFloat[0]    = toConvert[3];
  returnFloat[1]    = toConvert[2];
  returnFloat[2]    = toConvert[1];
  returnFloat[3]    = toConvert[0];

  return retVal;
}

u64 getScreenWidth()
{
  i32 w, h;
  SDL_GetWindowSize(g_renderer.window, &w, &h);

  return w;
}

u64 getScreenHeight()
{
  i32 w, h;
  SDL_GetWindowSize(g_renderer.window, &w, &h);

  return h;
}

u32 getTimeInMilliseconds()
{
  return SDL_GetTicks();
}

void getInfoStrings(char* msString, char* fpsString, long long* lastTick, long long* previousTick)
{
  memset(msString, 0, strlen(msString));
  memset(fpsString, 0, strlen(fpsString));

  char tempString[16];
  memcpy(msString, "ms: ", 4);
  memcpy(fpsString, "fps: ", 5);

  int ms = *lastTick - *previousTick;
  if (ms > 9999)
  {
    ms = 9999;
  }

  *previousTick = *lastTick;
  sprintf(tempString, "%d", ms);
  strcat(msString, tempString);

  float fps = (1.0f / (float)ms) * 1000;
  sprintf(tempString, "%d", (int)fps);
  strcat(fpsString, tempString);
}

f32 easeInCubic(f32 x)
{
  return x * x * x;
}

f32 easeOutCubic(f32 x)
{
  return 1 - pow(1 - x, 3);
}

f32 easeLinearly(f32 x)
{
  return x;
}
