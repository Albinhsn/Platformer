#ifndef GAME_UI_H
#define GAME_UI_H

#include "input.h"
#include "background.h"
#include "ui.h"

#define CONSOLE_NUMBER_OF_COMMANDS_VISIBLE 6

enum UIState
{
  UI_EXIT,
  UI_MAIN_MENU,
  UI_SETTINGS_MENU,
  UI_GAME_OVER,
  UI_GAME_RUNNING,
  UI_PAUSE_MENU,
  UI_CONSOLE
};
typedef enum UIState UIState;

struct ConsoleUI
{
  UIState     parent;
  UIComponent background;
  UIComponent consoleInput;
  u8          inputLength;
  u8*         input;
  u8**        sentCommands;
};
typedef struct ConsoleUI ConsoleUI;

struct GameRunningUI
{
  u32 mapIdx;
};
typedef struct GameRunningUI GameRunningUI;

struct GameOverUI
{
  ButtonUIComponent restartButton;
  ButtonUIComponent mainMenuButton;
};
typedef struct GameOverUI GameOverUI;

struct MainMenuUI
{
  ButtonUIComponent playButton;
  ButtonUIComponent exitButton;
  ButtonUIComponent settingsButton;
  Background background;
};
typedef struct MainMenuUI MainMenuUI;

struct PauseMenuUI
{
  ButtonUIComponent playButton;
  ButtonUIComponent mainMenuButton;
  ButtonUIComponent settingsButton;
};
typedef struct PauseMenuUI PauseMenuUI;

struct SettingsMenuUI
{
  UIState             parentState;
  ButtonUIComponent   returnButton;
  CheckboxUIComponent vsyncCheckbox;
  DropdownUIComponent screenSizeDropdown;
  SliderUIComponent   audioSlider;
};
typedef struct SettingsMenuUI SettingsMenuUI;

struct UI
{
  UIState         state;
  ConsoleUI*      console;
  GameOverUI*     gameOver;
  MainMenuUI*     mainMenu;
  PauseMenuUI*    pauseMenu;
  SettingsMenuUI* settingsMenu;
  GameRunningUI*  gameRunning;
};
typedef struct UI UI;

void              updateUIState(UI* ui, UIState newState, Timer* timer);
UIState           renderUI(UI* ui, InputState* inputState);
void              animate(f32* width, f32* height, UIAnimation* animation, bool hovers);
void              initUI(UI* ui, ConsoleUI* console, GameOverUI* gameOver, MainMenuUI* mainMenu, PauseMenuUI* pauseMenu, SettingsMenuUI* settings);

#endif
