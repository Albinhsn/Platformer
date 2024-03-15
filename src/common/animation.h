#ifndef ANIMATION_H
#define ANIMATION_H

#include "common.h"
#include "json.h"
#include "sta_string.h"


struct AnimationData
{
  String name;
  u64*   textureIds;
  u64    textureCount;
  u64    timer;
};
typedef struct AnimationData AnimationData;

struct Animation
{
  AnimationData* animationData;
  u64            lastUpdate;
  u64            currentTexture;
};
typedef struct Animation Animation;

extern AnimationData g_animationData[128];

void updateAnimation(Animation * animation);
void parseAnimationDataFromJson(Json * json);

#endif
