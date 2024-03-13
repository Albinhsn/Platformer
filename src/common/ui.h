#ifndef UI_H
#define UI_H

#include "common.h"
#include "vector.h"
#include "input.h"
#include <stdbool.h>

struct Animation
{
  u64 startedAnimation;
  u64 endedAnimation;
  u64 animationTimer;
  f32 initialWidth;
  f32 initialHeight;
  f32 maxSize;
  u32 functionIdx;
};

typedef struct Animation Animation;


struct UIComponent
{
  u32 textureIdx;
  f32 x;
  f32 y;
  f32 width;
  f32 height;
};
typedef struct UIComponent UIComponent;

struct ButtonUIComponent
{
  const char* text;
  f32         fontSize;
  f32         spaceSize;
  Color       color;
  UIComponent component;
  Animation   animation;
};
typedef struct ButtonUIComponent ButtonUIComponent;

struct CheckboxUIComponent
{
  UIComponent checkmark;
  UIComponent background;
  bool        toggled;
};
typedef struct CheckboxUIComponent CheckboxUIComponent;

struct DropdownUIComponent
{
  u32                itemCount;
  ButtonUIComponent  dropdownButton;
  ButtonUIComponent* items;
  void*              dropdownData;
  bool               toggled;
};
typedef struct DropdownUIComponent DropdownUIComponent;

struct SliderUIComponent
{
  UIComponent background;
  UIComponent bar;
  UIComponent slider;
  f32         value;
  f32         minValue;
  f32         maxValue;
};
typedef struct SliderUIComponent SliderUIComponent;

void                             initUIComponent(UIComponent* component, f32 x, f32 y, f32 width, f32 height, u32 textureIdx);
void                             initSlider(SliderUIComponent* slider, f32 initialValue, f32 minValue, f32 maxValue, f32 x, f32 y, f32 width, f32 height);
void                             initCheckbox(CheckboxUIComponent* checkbox, f32 x, f32 y, f32 width, f32 height, bool toggled);
void                             initButton(ButtonUIComponent* button, Color color, const char* text, f32 fontSize, f32 spaceSize, f32 x, f32 y, f32 width, f32 height, u32 textureIdx);
void initDropdown(DropdownUIComponent* slider, u32 itemCount, const char** itemText, void* dropdownData, Color color, const char* text, f32 fontSize, f32 spaceSize, f32 x, f32 y, f32 width,
                  f32 height);
void initAnimation(Animation* animation, f32 initialWidth, f32 initialHeight, u64 animationTimer, f32 maxSize, u32 functionIdx);
void animate(f32* width, f32* height, Animation* animation, bool hovers);
bool hovers(UIComponent component, InputState* inputState);
bool componentIsReleased(UIComponent component, InputState* inputState);
bool componentIsPressed(UIComponent component, InputState* inputState);
void updateSliderPosition(SliderUIComponent* slider, InputState* inputState);
void updateSliderValue(SliderUIComponent* slider);

#endif
