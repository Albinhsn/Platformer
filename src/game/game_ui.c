#include "game_ui.h"
#include "renderer.h"
#include "timer.h"
#include "ui.h"
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

static UIState                renderSettingsMenu(SettingsMenuUI* settingsMenu, InputState* inputState)
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
  animate(&pauseMenu->playButton.component.width, &pauseMenu->playButton.component.height, &pauseMenu->playButton.animation, hovers(pauseMenu->playButton.component, inputState));
  renderButton(&pauseMenu->playButton);
  if (componentIsReleased(pauseMenu->playButton.component, inputState))
  {
    return UI_GAME_RUNNING;
  }

  animate(&pauseMenu->mainMenuButton.component.width, &pauseMenu->mainMenuButton.component.height, &pauseMenu->mainMenuButton.animation, hovers(pauseMenu->mainMenuButton.component, inputState));
  renderButton(&pauseMenu->mainMenuButton);
  if (componentIsReleased(pauseMenu->mainMenuButton.component, inputState))
  {
    return UI_MAIN_MENU;
  }

  animate(&pauseMenu->settingsButton.component.width, &pauseMenu->settingsButton.component.height, &pauseMenu->settingsButton.animation, hovers(pauseMenu->settingsButton.component, inputState));
  renderButton(&pauseMenu->settingsButton);
  if (componentIsReleased(pauseMenu->settingsButton.component, inputState))
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

    if (strncmp((char*)console->input, "reset", 5) == 0)
    {
      setStateVariable("reset", 1);
    }

    if (strncmp((char*)console->input, "jump", 4) == 0)
    {
      u8 idx = 4;
      while (console->input[idx] == ' ')
      {
        idx++;
      }
      i32 res;
      u8  l;
      parseIntFromString(&res, (char*)&console->input[idx], &l);
      setStateVariable("jump", res);
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
  f32 buttonWidth  = getStateVariable("buttonSizeLargeWidth");
  f32 buttonHeight = getStateVariable("buttonSizeLargeHeight");
  f32 fontSize     = getStateVariable("fontFontSizeMedium");
  f32 spaceSize    = getStateVariable("fontSpaceSizeMedium");

  initButton(&menu->playButton, RED, "PLAY", fontSize, spaceSize, 0.0f, 31.0f, buttonWidth, buttonHeight, TEXTURE_GREY_BUTTON_05);
  initAnimation(&menu->playButton.animation, buttonWidth, buttonHeight, 500, 2.0f, 0);

  initButton(&menu->settingsButton, RED, "SETTINGS", fontSize, spaceSize, 0.0f, 0.0f, buttonWidth, buttonHeight, TEXTURE_GREY_BUTTON_05);
  initAnimation(&menu->settingsButton.animation, buttonWidth, buttonHeight, 500, 2.0f, 1);

  initButton(&menu->mainMenuButton, RED, "MAIN MENU", fontSize, spaceSize, 0.0f, -31.0f, buttonWidth, buttonHeight, TEXTURE_GREY_BUTTON_05);
  initAnimation(&menu->mainMenuButton.animation, buttonWidth, buttonHeight, 500, 2.0f, 2);
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
  initSlider(&settings->audioSlider, 50.0f, 0.0f, 100.0f, 0.0f, 60.0f, 65.0f, 8.0f, TEXTURE_GREY_BUTTON_05, TEXTURE_GREY_SLIDER_HORIZONTAL, TEXTURE_GREY_SLIDER_UP);

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

  initDropdown(&settings->screenSizeDropdown, itemCount, dropdownText, (void*)pairs, RED, "Resolution", fontSize, spaceSize, 60.0f, 20.0f, dropdownButtonWidth, dropdownButtonHeight,
               TEXTURE_GREY_BUTTON_05);
  initCheckbox(&settings->vsyncCheckbox, 0.0f, 0.0f, 6.0f, 8.0f, getStateVariable("vsync") != 0, TEXTURE_GREY_BOX, TEXTURE_GREY_CHECKMARK_GREY);

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
