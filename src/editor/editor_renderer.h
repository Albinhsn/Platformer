#ifndef EDITOR_RENDERER_H
#define EDITOR_RENDERER_H

#include "common.h"
#include "renderer.h"

enum TextureModel
{
  TEXTURE_FONT
};
typedef enum TextureModel TextureModel;

inline u32                getTextureId(TextureModel textureModel)
{
  return g_renderer.textures[textureModel].textureId;
}

#endif
