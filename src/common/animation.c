#include "animation.h"
#include "common.h"

AnimationData g_animationData[128];
u64           g_animationCount = 0;

void          updateAnimation(Animation* animation)
{
  u32 currentTick = getTimeInMilliseconds();
  if (currentTick - animation->animationData->timer > animation->lastUpdate)
  {
    animation->lastUpdate = currentTick;
    animation->currentTexture++;
    animation->currentTexture = animation->currentTexture % animation->animationData->textureCount;
  }
}

static void parseTextureIds(u64** textureIds_, JsonArray textures)
{
  u64* textureIds = (u64*)malloc(sizeof(u64) * textures.arraySize);
  for (u64 i = 0; i < textures.arraySize; i++)
  {
    textureIds[i] = getTileMapping(textures.values[i].string);
  }
  *textureIds_ = textureIds;
}

static void clearAnimationData()
{
  for (u32 i = 0; i < 128; i++)
  {
    g_animationData[i].textureIds    = 0;
    g_animationData[i].timer         = 0;
    g_animationData[i].textureCount  = 0;
    g_animationData[i].name.len      = 0;
    g_animationData[i].name.buffer   = 0;
    g_animationData[i].name.capacity = 0;
  }
}

void parseAnimationDataFromJson(Json* json)
{
  clearAnimationData();
  JsonObject head = json->obj;
  for (u64 i = 0; i < head.size; i++)
  {
    AnimationData* data   = &g_animationData[g_animationCount++];

    String         key    = head.keys[i];
    JsonObject     value  = head.values[i].obj;
    data->name            = key;
    JsonArray  textures   = (lookupJsonElement(&value, "textures"))->arr;
    JsonValue* timerValue = lookupJsonElement(&value, "timer");

    data->textureCount    = textures.arraySize;
    parseTextureIds(&data->textureIds, textures);
    data->timer = timerValue->number;
  }
}
