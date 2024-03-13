#ifndef TIMER_H
#define TIMER_H

#include "common.h"
#include <stdbool.h>

struct Timer
{
  u64  lastTick;
  u64  serverTicks;
  bool running;
};
typedef struct Timer Timer;

void                 resetTimer(Timer *timer);
void                 updateTimer(Timer *timer);
void                 startTimer(Timer *timer);
void                 stopTimer(Timer *timer);

#endif
