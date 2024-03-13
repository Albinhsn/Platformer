#include "ui.h"
#include "common.h"
#include "renderer.h"
#include <SDL2/SDL_video.h>
#include <string.h>

void updateUIState(UI* ui, UIState newState, Timer* timer)
{

  if (ui->state == UI_GAME_RUNNING && newState != UI_GAME_RUNNING)
  {
    stopTimer(timer);
  }

  if (ui->state != UI_SETTINGS_MENU && newState == UI_SETTINGS_MENU)
  {
    ui->settingsMenu->parentState = ui->state;
  }

  if (ui->state == UI_GAME_OVER && newState != UI_GAME_OVER)
  {
  }

  if (ui->state != UI_CONSOLE && newState == UI_CONSOLE)
  {
    ui->console->parent = ui->state;
  }

  if (newState == UI_GAME_RUNNING && !timer->running)
  {
    startTimer(timer);
  }
  ui->state = newState;
}

static void initUIComponent(UIComponent* component, f32 x, f32 y, f32 width, f32 height, u32 textureIdx)
{
  component->textureIdx = textureIdx;
  component->x          = x;
  component->y          = y;
  component->width      = width;
  component->height     = height;
}

static void initSlider(SliderUIComponent* slider, f32 initialValue, f32 minValue, f32 maxValue, f32 x, f32 y, f32 width, f32 height)
{
  slider->minValue = minValue;
  slider->maxValue = maxValue;
  slider->value    = initialValue;
  initUIComponent(&slider->background, x, y, width, height, TEXTURE_GREY_BUTTON_05);
  initUIComponent(&slider->bar, x, y, width * 0.9f, height, TEXTURE_GREY_SLIDER_HORIZONTAL);
  initUIComponent(&slider->slider, x, y, width * 0.1f, height * 0.9f, TEXTURE_GREY_SLIDER_UP);
}

static void initCheckbox(CheckboxUIComponent* checkbox, f32 x, f32 y, f32 width, f32 height, bool toggled)
{
  checkbox->toggled = toggled;
  initUIComponent(&checkbox->background, x, y, width, height, TEXTURE_GREY_BOX);
  initUIComponent(&checkbox->checkmark, x, y, width, height, TEXTURE_GREY_CHECKMARK_GREY);
}

static void initButton(ButtonUIComponent* button, Color color, const char* text, f32 fontSize, f32 spaceSize, f32 x, f32 y, f32 width, f32 height, u32 textureIdx)
{
  button->component.x          = x;
  button->component.y          = y;
  button->component.width      = width;
  button->component.height     = height;
  button->component.textureIdx = textureIdx;
  button->color                = color;
  button->text                 = text;
  button->fontSize             = fontSize;
  button->spaceSize            = spaceSize;
}

static void initDropdown(DropdownUIComponent* slider, u32 itemCount, const char** itemText, void* dropdownData, Color color, const char* text, f32 fontSize, f32 spaceSize, f32 x, f32 y, f32 width,
                         f32 height)
{
  slider->toggled      = false;
  slider->itemCount    = itemCount;
  slider->dropdownData = dropdownData;

  initButton(&slider->dropdownButton, color, text, fontSize, spaceSize, x, y, width, height, TEXTURE_GREY_BUTTON_05);

  slider->items = (ButtonUIComponent*)malloc(sizeof(ButtonUIComponent) * itemCount);
  for (u32 i = 0; i < itemCount; i++)
  {
    y -= 2.0f * height;
    initButton(&slider->items[i], color, itemText[i], fontSize, spaceSize, x, y, width, height, TEXTURE_GREY_BUTTON_05);
  }
}

static void initAnimation(Animation* animation, f32 initialWidth, f32 initialHeight, u64 animationTimer, f32 maxSize, u32 functionIdx)
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

static void animateIn(f32* width, f32* height, Animation* animation)
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

static void animateOut(f32* width, f32* height, Animation* animation)
{
  u64 tickDifference          = getTimeInMilliseconds() - animation->endedAnimation;
  f32 increasePerMs           = animation->maxSize / (f32)animation->animationTimer;
  f32 increase                = animationFuncs[animation->functionIdx](1.0f - MIN(increasePerMs * tickDifference, 1));

  animation->startedAnimation = 0;
  *width                      = animation->initialWidth + animation->maxSize * increase;
  *height                     = animation->initialHeight + animation->maxSize * increase;
}

void animate(f32* width, f32* height, Animation* animation, bool hovers)
{
  hovers ? animateIn(width, height, animation) : animateOut(width, height, animation);
}

static bool hovers(UIComponent component, InputState* inputState)
{
  f32 mouseX = ((inputState->mouseX / (f32)getScreenWidth()) * 2.0f - 1.0f) * 100.0f;
  f32 mouseY = ((inputState->mouseY / (f32)getScreenHeight()) * 2.0f - 1.0f) * -100.0f;

  f32 minX   = component.x - component.width;
  f32 maxX   = component.x + component.width;
  f32 minY   = component.y - component.height;
  f32 maxY   = component.y + component.height;

  return minX <= mouseX && mouseX <= maxX && minY <= mouseY && mouseY <= maxY;
}

static bool componentIsReleased(UIComponent component, InputState* inputState)
{
  if (!inputState->mr_1)
  {
    return false;
  }
  return hovers(component, inputState);
}

static bool componentIsPressed(UIComponent component, InputState* inputState)
{
  if (!inputState->md_1)
  {
    return false;
  }
  return hovers(component, inputState);
}

static UIState renderMainMenu(MainMenuUI* mainMenu, InputState* inputState)
{
  animate(&mainMenu->playButton.component.width, &mainMenu->playButton.component.height, &mainMenu->playButton.animation, hovers(mainMenu->playButton.component, inputState));
  renderButton(&mainMenu->playButton);
  if (componentIsReleased(mainMenu->playButton.component, inputState))
  {
    return UI_GAME_RUNNING;
  }

  animate(&mainMenu->settingsButton.component.width, &mainMenu->settingsButton.component.height, &mainMenu->settingsButton.animation, hovers(mainMenu->settingsButton.component, inputState));
  renderButton(&mainMenu->settingsButton);
  if (componentIsReleased(mainMenu->settingsButton.component, inputState))
  {
    return UI_SETTINGS_MENU;
  }

  animate(&mainMenu->exitButton.component.width, &mainMenu->exitButton.component.height, &mainMenu->exitButton.animation, hovers(mainMenu->exitButton.component, inputState));
  renderButton(&mainMenu->exitButton);
  if (componentIsReleased(mainMenu->exitButton.component, inputState))
  {
    return UI_EXIT;
  }
  return UI_MAIN_MENU;
}

struct ScreenSizePair
{
  i32 w;
  i32 h;
};
typedef struct ScreenSizePair ScreenSizePair;

static void                   updateWindowSize(i32 width, i32 height)
{
  SDL_SetWindowSize(g_renderer.window, width, height);
  glViewport(0, 0, width, height);
}

static void updateSliderPosition(SliderUIComponent* slider, InputState* inputState)
{
  slider->slider.x = ((inputState->mouseX / (f32)getScreenWidth()) * 2.0f - 1.0f) * 100.0f;

  f32 offset       = 10.0f;
  slider->slider.x = MAX(slider->slider.x, (slider->background.x - slider->background.width + offset));
  slider->slider.x = MIN(slider->slider.x, (slider->background.x + slider->background.width - offset));
}
static void updateSliderValue(SliderUIComponent* slider)
{
  f32 minSliderX = slider->background.x - slider->background.width + 10.0f + slider->slider.width;
  f32 maxSliderX = slider->background.x - slider->background.width + 10.0f + slider->slider.width;

  slider->value  = slider->value + (slider->maxValue - slider->minValue) * (slider->slider.x - minSliderX) / (maxSliderX - minSliderX);
}

static UIState renderSettingsMenu(SettingsMenuUI* settingsMenu, InputState* inputState)
{
  f32 fontSize  = getStateVariable("fontFontSizeSmall");
  f32 spaceSize = getStateVariable("fontSpaceSizeSmall");

  // RETURN
  if (componentIsReleased(settingsMenu->returnButton.component, inputState))
  {
    return settingsMenu->parentState;
  }
  renderButton(&settingsMenu->returnButton);

  // CHECKBOX
  if (componentIsReleased(settingsMenu->vsyncCheckbox.background, inputState))
  {
    settingsMenu->vsyncCheckbox.toggled = !settingsMenu->vsyncCheckbox.toggled;
    SDL_GL_SetSwapInterval(settingsMenu->vsyncCheckbox.toggled);
  }
  f32 endX = settingsMenu->vsyncCheckbox.background.x - settingsMenu->vsyncCheckbox.background.width;
  renderTextEndsAt("vsync", &WHITE, endX, 0, fontSize, spaceSize);
  renderCheckbox(&settingsMenu->vsyncCheckbox);

  // SCREEN SIZE DROPDOWN
  DropdownUIComponent* dropdown = &settingsMenu->screenSizeDropdown;
  if (componentIsReleased(dropdown->dropdownButton.component, inputState))
  {
    dropdown->toggled = !dropdown->toggled;
  }
  renderDropdown(dropdown);
  if (dropdown->toggled)
  {
    ScreenSizePair* data = (ScreenSizePair*)dropdown->dropdownData;
    for (int i = 0; i < dropdown->itemCount; i++)
    {
      if (componentIsReleased(dropdown->items[i].component, inputState))
      {
        updateWindowSize(data[i].w, data[i].h);
        dropdown->toggled = false;
        break;
      }
    }
  }

  // AUDIO SLIDER
  SliderUIComponent* slider = &settingsMenu->audioSlider;
  if (componentIsPressed(slider->slider, inputState))
  {
    updateSliderPosition(slider, inputState);
    updateSliderValue(slider);
  }
  renderSlider(slider);
  return UI_SETTINGS_MENU;
}

static UIState renderGameOver(GameOverUI* gameOver, InputState* inputState)
{
  f32 fontSize  = FONT_FONT_SIZE_LARGE;
  f32 spaceSize = FONT_SPACE_SIZE_LARGE;
  renderTextCentered("GAME OVER", &WHITE, 0, 90.0f - FONT_FONT_SIZE_LARGE, fontSize, spaceSize);

  renderButton(&gameOver->restartButton);
  if (componentIsReleased(gameOver->restartButton.component, inputState))
  {
    return UI_GAME_RUNNING;
  }

  renderButton(&gameOver->mainMenuButton);
  if (componentIsReleased(gameOver->mainMenuButton.component, inputState))
  {
    return UI_MAIN_MENU;
  }
  return UI_GAME_OVER;
}

static UIState renderGameRunning(InputState* inputState)
{

  if (inputState->keyboardStateRelease[ASCII_ESCAPE])
  {
    return UI_PAUSE_MENU;
  }

  return UI_GAME_RUNNING;
}

static UIState renderPauseMenu(PauseMenuUI* pauseMenu, InputState* inputState)
{
  renderButton(&pauseMenu->playButton);
  if (componentIsPressed(pauseMenu->playButton.component, inputState))
  {
    return UI_GAME_RUNNING;
  }

  renderButton(&pauseMenu->mainMenuButton);
  if (componentIsPressed(pauseMenu->mainMenuButton.component, inputState))
  {
    return UI_MAIN_MENU;
  }

  renderButton(&pauseMenu->settingsButton);
  if (componentIsPressed(pauseMenu->settingsButton.component, inputState))
  {
    return UI_SETTINGS_MENU;
  }

  return UI_PAUSE_MENU;
}

static UIState handleConsoleInput(ConsoleUI* console, InputState* inputState)
{
  if (inputState->keyboardStateRelease[ASCII_ESCAPE])
  {
    return console->parent;
  }
  for (i32 i = 0; i < 127; i++)
  {
    if (inputState->keyboardStateRelease[i] && (isalpha(i) || isdigit(i)))
    {
      console->input[console->inputLength] = (char)i;
      console->inputLength++;
    }
  }
  if (inputState->keyboardStateRelease[ASCII_SPACE])
  {
    console->input[console->inputLength] = ' ';
    console->inputLength++;
  }

  if (inputState->keyboardStateRelease[ASCII_BACKSPACE] && console->inputLength > 0)
  {
    console->inputLength--;
    console->input[console->inputLength] = 0;
  }

  if (inputState->keyboardStateRelease[ASCII_RETURN])
  {

    if (strncmp((char*)console->input, "exit", 4) == 0)
    {
      return UI_EXIT;
    }
    if (strncmp((char*)console->input, "god", 3) == 0)
    {
      setStateVariable("god", 1.0f);
    }
    if (strncmp((char*)console->input, "meteor", 6) == 0)
    {
      u8 idx = 6;
      while (console->input[idx] == ' ')
      {
        idx++;
      }
      i32 res;
      u8  l;
      parseIntFromString(&res, (char*)&console->input[idx], &l);
      setStateVariable("numberOfMeteors", res);
    }

    for (i32 i = CONSOLE_NUMBER_OF_COMMANDS_VISIBLE - 2; i >= 0; i--)
    {
      memcpy(console->sentCommands[i + 1], console->sentCommands[i], 32);
    }
    memcpy(console->sentCommands[0], console->input, 32);
    memset(console->input, 0, 32);
    console->inputLength = 0;
  }

  return UI_CONSOLE;
}

static void renderSentCommandsConsole(ConsoleUI* console)
{
  f32 fontSize  = FONT_FONT_SIZE_MEDIUM;
  f32 spaceSize = FONT_SPACE_SIZE_MEDIUM;
  f32 x         = console->consoleInput.x - console->consoleInput.width;
  f32 y         = console->consoleInput.y;

  renderTextStartsAt((const char*)console->input, &BLACK, x, y, fontSize, spaceSize);
  for (u32 i = 0; i < CONSOLE_NUMBER_OF_COMMANDS_VISIBLE; i++)
  {
    y += 15.0f;
    renderTextStartsAt((const char*)console->sentCommands[i], &RED, x, y, fontSize, spaceSize);
  }
}

static UIState renderConsole(ConsoleUI* console, InputState* inputState)
{
  renderComponent(&console->background);
  renderComponent(&console->consoleInput);
  renderSentCommandsConsole(console);

  UIState state = handleConsoleInput(console, inputState);
  if (state != UI_CONSOLE)
  {
    for (i32 i = 0; i < CONSOLE_NUMBER_OF_COMMANDS_VISIBLE; i++)
    {
      memset(console->sentCommands[i], 0, 32);
    }
    memset(console->input, 0, 32);
    console->inputLength = 0;
  }

  return state;
}

UIState renderUI(UI* ui, InputState* inputState)
{

  switch (ui->state)
  {
  case UI_MAIN_MENU:
  {
    return renderMainMenu(ui->mainMenu, inputState);
  }
  case UI_SETTINGS_MENU:
  {
    return renderSettingsMenu(ui->settingsMenu, inputState);
  }
  case UI_GAME_OVER:
  {
    return renderGameOver(ui->gameOver, inputState);
  }
  case UI_GAME_RUNNING:
  {
    return renderGameRunning(inputState);
  }
  case UI_PAUSE_MENU:
  {
    return renderPauseMenu(ui->pauseMenu, inputState);
  }
  case UI_CONSOLE:
  {
    return renderConsole(ui->console, inputState);
  }
  default:
  {
  }
  }

  return ui->state;
}

void initConsoleUI(ConsoleUI* console)
{
  console->inputLength = 0;
  console->input       = (u8*)malloc(sizeof(u8) * TEXT_MAX_LENGTH);
  memset(console->input, 0, TEXT_MAX_LENGTH);
  console->sentCommands = (u8**)malloc(sizeof(u8*) * CONSOLE_NUMBER_OF_COMMANDS_VISIBLE);
  for (u32 i = 0; i < CONSOLE_NUMBER_OF_COMMANDS_VISIBLE; i++)
  {
    console->sentCommands[i] = (u8*)malloc(sizeof(u8) * TEXT_MAX_LENGTH);
    memset(console->sentCommands[i], 0, TEXT_MAX_LENGTH);
  }
  console->parent                  = UI_CONSOLE;

  console->background.x            = 0;
  console->background.y            = 0;
  console->background.width        = 50.0f;
  console->background.height       = 50.0f;
  console->background.textureIdx   = TEXTURE_GREY_BUTTON_05;

  f32 fontSize                     = 6.0f;

  console->consoleInput.textureIdx = TEXTURE_GREY_BUTTON_14;
  console->consoleInput.x          = 0.0f;
  console->consoleInput.y          = -42.0f;
  console->consoleInput.width      = 50.0f;
  console->consoleInput.height     = fontSize * 1.2f;
}

void initGameOverUI(GameOverUI* gameOver)
{
  f32 buttonWidth  = BUTTON_SIZE_MEDIUM_WIDTH;
  f32 buttonHeight = BUTTON_SIZE_MEDIUM_HEIGHT;
  f32 fontSize     = FONT_FONT_SIZE_MEDIUM;
  f32 spaceSize    = FONT_SPACE_SIZE_MEDIUM;

  initButton(&gameOver->restartButton, RED, "RESTART", spaceSize, fontSize, 0.0f, 0.0f, buttonWidth, buttonHeight, TEXTURE_GREY_BUTTON_05);
  initButton(&gameOver->mainMenuButton, RED, "MAIN MENU", spaceSize, fontSize, 0.0f, -2 * buttonHeight, buttonWidth, buttonHeight, TEXTURE_GREY_BUTTON_05);
}

void initMainMenuUI(MainMenuUI* mainMenu)
{
  f32 buttonWidth  = getStateVariable("buttonSizeLargeWidth");
  f32 buttonHeight = getStateVariable("buttonSizeLargeHeight");
  f32 fontSize     = getStateVariable("fontFontSizeMedium");
  f32 spaceSize    = getStateVariable("fontSpaceSizeMedium");

  initButton(&mainMenu->playButton, RED, "PLAY", fontSize, spaceSize, 0.0f, 31.0f, buttonWidth, buttonHeight, TEXTURE_GREY_BUTTON_05);
  initAnimation(&mainMenu->playButton.animation, buttonWidth, buttonHeight, 500, 2.0f, 0);

  initButton(&mainMenu->settingsButton, RED, "SETTINGS", fontSize, spaceSize, 0.0f, 0.0f, buttonWidth, buttonHeight, TEXTURE_GREY_BUTTON_05);
  initAnimation(&mainMenu->settingsButton.animation, buttonWidth, buttonHeight, 500, 2.0f, 1);

  initButton(&mainMenu->exitButton, RED, "EXIT", fontSize, spaceSize, 0.0f, -31.0f, buttonWidth, buttonHeight, TEXTURE_GREY_BUTTON_05);
  initAnimation(&mainMenu->exitButton.animation, buttonWidth, buttonHeight, 500, 2.0f, 2);
}

void initPauseMenuUI(PauseMenuUI* menu)
{
  f32 buttonWidth  = BUTTON_SIZE_MEDIUM_WIDTH;
  f32 buttonHeight = BUTTON_SIZE_MEDIUM_HEIGHT;
  f32 fontSize     = FONT_FONT_SIZE_MEDIUM;
  f32 spaceSize    = FONT_SPACE_SIZE_MEDIUM;

  initButton(&menu->playButton, RED, "PLAY", spaceSize, fontSize, 0.0f, 2 * buttonHeight, buttonWidth, buttonHeight, TEXTURE_GREY_BUTTON_05);
  initButton(&menu->settingsButton, RED, "SETTINGS", spaceSize, fontSize, 0.0f, 0.0f, buttonWidth, buttonHeight, TEXTURE_GREY_BUTTON_05);
  initButton(&menu->mainMenuButton, RED, "MAIN MENU", spaceSize, fontSize, 0.0f, -2 * buttonHeight, buttonWidth, buttonHeight, TEXTURE_GREY_BUTTON_05);
}

void initSettingsUI(SettingsMenuUI* settings)
{
  f32 buttonWidth          = BUTTON_SIZE_LARGE_WIDTH;
  f32 buttonHeight         = BUTTON_SIZE_LARGE_HEIGHT;
  f32 fontSize             = FONT_FONT_SIZE_MEDIUM;
  f32 spaceSize            = FONT_SPACE_SIZE_MEDIUM;
  f32 dropdownButtonWidth  = BUTTON_SIZE_MEDIUM_WIDTH;
  f32 dropdownButtonHeight = BUTTON_SIZE_MEDIUM_HEIGHT;

  initButton(&settings->returnButton, RED, "RETURN", fontSize, spaceSize, 0.0f, -40.0f, buttonWidth, buttonHeight, TEXTURE_GREY_BUTTON_05);
  initSlider(&settings->audioSlider, 50.0f, 0.0f, 100.0f, 0.0f, 60.0f, 65.0f, 8.0f);

  u32             itemCount   = 4;
  ScreenSizePair* pairs       = (ScreenSizePair*)malloc(sizeof(ScreenSizePair) * itemCount);
  pairs[0]                    = (ScreenSizePair){.w = 1920, .h = 1080};
  pairs[1]                    = (ScreenSizePair){.w = 1600, .h = 900};
  pairs[2]                    = (ScreenSizePair){.w = 1024, .h = 768};
  pairs[3]                    = (ScreenSizePair){.w = 620, .h = 480};
  const char* dropdownText[4] = {
      "1920x1080",
      "1600x900",
      "1024x768",
      "620x480",
  };

  initDropdown(&settings->screenSizeDropdown, itemCount, dropdownText, (void*)pairs, RED, "Resolution", fontSize, spaceSize, 60.0f, 20.0f, dropdownButtonWidth, dropdownButtonHeight);
  initCheckbox(&settings->vsyncCheckbox, 0.0f, 0.0f, 6.0f, 8.0f, getStateVariable("vsync") != 0);

  settings->parentState = UI_MAIN_MENU;
}

void initUI(UI* ui, ConsoleUI* console, GameOverUI* gameOver, MainMenuUI* mainMenu, PauseMenuUI* pauseMenu, SettingsMenuUI* settings)
{
  initConsoleUI(console);
  ui->console = console;

  initGameOverUI(gameOver);
  ui->gameOver = gameOver;

  initMainMenuUI(mainMenu);
  ui->mainMenu = mainMenu;

  initPauseMenuUI(pauseMenu);
  ui->pauseMenu = pauseMenu;

  initSettingsUI(settings);
  ui->settingsMenu = settings;
}
