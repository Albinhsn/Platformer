#ifndef ANIMATION_H
#define ANIMATION_H

#include "common.h"
#include "sta_string.h"

struct AnimationData
{
  u64* textureIds;
  u64  textureCount;
  u64  timer;
};
typedef struct AnimationData AnimationData;

struct Animation
{
  AnimationData* animationData;
  u64            lastUpdate;
  u64            currentTexture;
  bool           shouldBeUpdated;
};
typedef struct Animation Animation;

void                     initAnimation(Animation* animation, AnimationData* data);
void                     updateAnimation(Animation* animation, Timer* timer);

#endif
