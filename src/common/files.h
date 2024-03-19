#ifndef IMAGE_H
#define IMAGE_H
#include "common.h"
#include <GL/gl.h>
#include <stdbool.h>

#define IDLE_STATE     0
#define RUNNING_STATE  1
#define ATTACK_1_STATE 2
#define ATTACK_2_STATE 3
#define ATTACK_3_STATE 4

struct PNG
{
  u32 width;
  u32 height;
  u8* data;
  u8  bpp;
};
typedef struct PNG PNG;

struct Image
{
  u32 width, height;
  u8  bpp;
  u8* data;
};
typedef struct Image Image;

struct TargaHeader
{
  union
  {
    u8 header[18];
    struct
    {
      u8  charactersInIdentificationField;
      u8  colorMapType;
      u8  imageType;
      u8  colorMapSpec[5];
      u16 xOrigin;
      u16 yOrigin;
      u16 width;
      u16 height;
      u8  imagePixelSize;
      u8  imageDescriptor;
    };
  };
};
typedef struct TargaHeader TargaHeader;

bool                       parseTarga(u8** data, u32* width, u32* height, const char* filename);
bool                       readFile(char** buffer, int* len, const char* fileName);
bool                       parsePNG(u8** data, u32* width, u32* height, const char* filename);

#endif
