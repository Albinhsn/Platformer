#include "timer.h"
#include <SDL2/SDL_timer.h>

void resetTimer(Timer *timer)
{
  timer->running     = false;
  timer->lastTick    = 0;
  timer->serverTicks = 0;
}
void updateTimer(Timer *timer)
{
  u64 tick = SDL_GetTicks();
  timer->lastTick += tick - timer->serverTicks;
  timer->serverTicks = tick;
}
void startTimer(Timer *timer)
{
  timer->running     = true;
  timer->serverTicks = SDL_GetTicks();
}
void stopTimer(Timer *timer)
{
  timer->running = false;
}
