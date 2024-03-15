#ifndef COMMON_H
#define COMMON_H

#include "sta_string.h"
#include <stdbool.h>
#include <stdint.h>
#include <sys/time.h>

#define BUTTON_SIZE_SMALL_WIDTH   18
#define BUTTON_SIZE_SMALL_HEIGHT  6
#define BUTTON_SIZE_MEDIUM_WIDTH  32
#define BUTTON_SIZE_MEDIUM_HEIGHT 10
#define BUTTON_SIZE_LARGE_WIDTH   40
#define BUTTON_SIZE_LARGE_HEIGHT  10

#define FONT_FONT_SIZE_SMALL      3
#define FONT_SPACE_SIZE_SMALL     5
#define FONT_FONT_SIZE_MEDIUM     3
#define FONT_SPACE_SIZE_MEDIUM    10
#define FONT_FONT_SIZE_LARGE      3
#define FONT_SPACE_SIZE_LARGE     10

#define DEBUG_INPUT               false
#define DEBUG_PLAYER              false

#define DEFAULT_SCREENWIDTH       1024
#define DEFAULT_SCREENHEIGHT      780

#define INITIAL_HP                3

#define PLAYER_GCD                500
#define PLAYER_MS                 1.0f

#define MIN(fst, snd)             (fst < snd ? fst : snd)
#define MAX(fst, snd)             (fst > snd ? fst : snd)

#define M_PI                      3.14159265359

#define RED                       ((Vec4f32){1.0f, 0.0f, 0.0f, 1.0f})
#define YELLOW                    ((Vec4f32){1.0f, 1.0f, 0.0f, 1.0f})
#define GREEN                     ((Vec4f32){0.0f, 1.0f, 0.0f, 1.0f})
#define CYAN                      ((Vec4f32){0.0f, 1.0f, 1.0f, 1.0f})
#define PURPLE                    ((Vec4f32){1.0f, 0.0f, 1.0f, 1.0f})
#define BLUE                      ((Vec4f32){1.0f, 0.0f, 0.0f, 1.0f})
#define BLACK                     ((Vec4f32){0.0f, 0.0f, 0.0f, 1.0f})
#define WHITE                     ((Vec4f32){1.0f, 1.0f, 1.0f, 1.0f})

#define ASCII_ESCAPE              27
#define ASCII_RETURN              13
#define ASCII_SPACE               32
#define ASCII_BACKSPACE           8

#define FONT_DATA_LOCATION        "./Assets/Fonts/font01.txt"

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float    f32;
typedef double   f64;

typedef int8_t   i8;
typedef int16_t  i16;
typedef int      i32;
typedef int64_t  i64;

struct Timer;
typedef struct Timer Timer;

bool                 shouldHandleUpdates(Timer* timer, u64* lastUpdated);
void                 parseFloatFromString(float* dest, char* source, u8* length);
void                 parseIntFromString(int* dest, char* source, u8* length);
f32                  getRandomFloat(f32 min, f32 max);
u64                  getScreenWidth();
u64                  getScreenHeight();
f32                  convertFloatToBE(f32 f);
u32                  getTimeInMilliseconds();
void                 loadTileMapMapping();
void                 loadStateVariables();
void                 setTileMapping(const char* key, f32 value);
f32                  getTileMapping(String key);
void                 setStateVariable(const char* key, f32 value);
f32                  getStateVariable(const char* key);
void                 getInfoStrings(char* fpsString, char* msString, long long* lastTick, long long* previousTick);
f32                  easeInCubic(f32 x);
f32                  easeOutCubic(f32 x);
f32                  easeLinearly(f32 x);

#endif
