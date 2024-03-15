#include "ui.h"
#include "common.h"
#include <SDL2/SDL_video.h>

void initUIComponent(UIComponent* component, f32 x, f32 y, f32 width, f32 height, u32 textureIdx)
{
  component->textureIdx = textureIdx;
  component->x          = x;
  component->y          = y;
  component->width      = width;
  component->height     = height;
}

void initSlider(SliderUIComponent* slider, f32 initialValue, f32 minValue, f32 maxValue, f32 x, f32 y, f32 width, f32 height, u32 backgroundTextureIdx, u32 barTextureIdx, u32 sliderTextureIdx)
{
  slider->minValue = minValue;
  slider->maxValue = maxValue;
  slider->value    = initialValue;
  initUIComponent(&slider->background, x, y, width, height, backgroundTextureIdx);
  initUIComponent(&slider->bar, x, y, width * 0.9f, height, barTextureIdx);
  initUIComponent(&slider->slider, x, y, width * 0.1f, height * 0.9f, sliderTextureIdx);
}

void initCheckbox(CheckboxUIComponent* checkbox, f32 x, f32 y, f32 width, f32 height, bool toggled, u32 backgroundTextureIdx, u32 checkmarkTextureIdx)
{
  checkbox->toggled = toggled;
  initUIComponent(&checkbox->background, x, y, width, height, backgroundTextureIdx);
  initUIComponent(&checkbox->checkmark, x, y, width, height, checkmarkTextureIdx);
}

void initButton(ButtonUIComponent* button, Color color, const char* text, f32 fontSize, f32 spaceSize, f32 x, f32 y, f32 width, f32 height, u32 textureIdx)
{
  initUIComponent(&button->component, x, y, width, height, textureIdx);
  button->color     = color;
  button->text      = text;
  button->fontSize  = fontSize;
  button->spaceSize = spaceSize;
}

void initDropdown(DropdownUIComponent* slider, u32 itemCount, const char** itemText, void* dropdownData, Color color, const char* text, f32 fontSize, f32 spaceSize, f32 x, f32 y, f32 width,
                  f32 height, u32 dropdownButtonTextureIdx)
{
  slider->toggled      = false;
  slider->itemCount    = itemCount;
  slider->dropdownData = dropdownData;

  initButton(&slider->dropdownButton, color, text, fontSize, spaceSize, x, y, width, height, dropdownButtonTextureIdx);

  slider->items = (ButtonUIComponent*)malloc(sizeof(ButtonUIComponent) * itemCount);
  for (u32 i = 0; i < itemCount; i++)
  {
    y -= 2.0f * height;
    initButton(&slider->items[i], color, itemText[i], fontSize, spaceSize, x, y, width, height, dropdownButtonTextureIdx);
  }
}

void initUIAnimation(UIAnimation* animation, f32 initialWidth, f32 initialHeight, u64 animationTimer, f32 maxSize, u32 functionIdx)
{
  animation->initialWidth     = initialWidth;
  animation->initialHeight    = initialHeight;
  animation->animationTimer   = animationTimer;
  animation->startedAnimation = 0;
  animation->endedAnimation   = 0;
  animation->maxSize          = maxSize;
  animation->functionIdx      = functionIdx;
}

static f32  (*animationFuncs[])(float) = {easeLinearly, easeInCubic, easeOutCubic};

static void animateIn(f32* width, f32* height, UIAnimation* animation)
{
  u32 tick = getTimeInMilliseconds();
  if (animation->startedAnimation == 0)
  {
    animation->startedAnimation = tick;
  }

  u64 tickDifference        = tick - animation->startedAnimation;
  f32 increasePerMs         = animation->maxSize / animation->animationTimer;
  f32 increase              = animationFuncs[animation->functionIdx](MIN(increasePerMs * tickDifference, 1));

  *width                    = animation->initialWidth + animation->maxSize * increase;
  *height                   = animation->initialHeight + animation->maxSize * increase;
  animation->endedAnimation = tick;
}

static void animateOut(f32* width, f32* height, UIAnimation* animation)
{
  u64 tickDifference          = getTimeInMilliseconds() - animation->endedAnimation;
  f32 increasePerMs           = animation->maxSize / (f32)animation->animationTimer;
  f32 increase                = animationFuncs[animation->functionIdx](1.0f - MIN(increasePerMs * tickDifference, 1));

  animation->startedAnimation = 0;
  *width                      = animation->initialWidth + animation->maxSize * increase;
  *height                     = animation->initialHeight + animation->maxSize * increase;
}

void animate(f32* width, f32* height, UIAnimation* animation, bool hovers)
{
  hovers ? animateIn(width, height, animation) : animateOut(width, height, animation);
}

bool hovers(UIComponent component, InputState* inputState)
{
  f32 mouseX = ((inputState->mouseX / (f32)getScreenWidth()) * 2.0f - 1.0f) * 100.0f;
  f32 mouseY = ((inputState->mouseY / (f32)getScreenHeight()) * 2.0f - 1.0f) * -100.0f;

  f32 minX   = component.x - component.width;
  f32 maxX   = component.x + component.width;
  f32 minY   = component.y - component.height;
  f32 maxY   = component.y + component.height;

  return minX <= mouseX && mouseX <= maxX && minY <= mouseY && mouseY <= maxY;
}

bool componentIsReleased(UIComponent component, InputState* inputState)
{
  if (!inputState->mr_1)
  {
    return false;
  }
  return hovers(component, inputState);
}

bool componentIsPressed(UIComponent component, InputState* inputState)
{
  if (!inputState->md_1)
  {
    return false;
  }
  return hovers(component, inputState);
}

void updateSliderPosition(SliderUIComponent* slider, InputState* inputState)
{
  slider->slider.x = ((inputState->mouseX / (f32)getScreenWidth()) * 2.0f - 1.0f) * 100.0f;

  f32 offset       = 10.0f;
  slider->slider.x = MAX(slider->slider.x, (slider->background.x - slider->background.width + offset));
  slider->slider.x = MIN(slider->slider.x, (slider->background.x + slider->background.width - offset));
}
void updateSliderValue(SliderUIComponent* slider)
{
  f32 minSliderX = slider->background.x - slider->background.width + 10.0f + slider->slider.width;
  f32 maxSliderX = slider->background.x - slider->background.width + 10.0f + slider->slider.width;

  slider->value  = slider->value + (slider->maxValue - slider->minValue) * (slider->slider.x - minSliderX) / (maxSliderX - minSliderX);
}
