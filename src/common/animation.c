#include "animation.h"
#include "common.h"
#include "timer.h"

void          updateAnimation(Animation* animation, Timer* timer)
{
  u32 currentTick = timer->lastTick;
  if (currentTick - animation->animationData->timer > animation->lastUpdate)
  {
    animation->lastUpdate = currentTick;
    animation->currentTexture++;
    animation->currentTexture = animation->currentTexture % animation->animationData->textureCount;
  }
}

void initAnimation(Animation* animation, AnimationData* data)
{
  animation->animationData  = data;
  animation->lastUpdate     = 0;
  animation->currentTexture = 0;
}
