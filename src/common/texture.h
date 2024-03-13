#ifndef TEXTURE_H
#define TEXTURE_H

#include "common.h"

struct Texture
{
  u32 textureId;
  u32 width;
  u32 height;
  u8* data;
};
typedef struct Texture Texture;

void parseTexture(Texture * texture);

#endif
