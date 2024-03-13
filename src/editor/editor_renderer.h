#ifndef EDITOR_RENDERER_H
#define EDITOR_RENDERER_H

#include "common.h"
#include "renderer.h"

enum TextureModel
{
  TEXTURE_FONT,
  TEXTURE_TILES,
  TEXTURE_BACKGROUNDS,
  TEXTURE_CHARACTERS
};
typedef enum TextureModel TextureModel;

void                      initTiledTextures();
void                      renderTextureTile(f32 x, f32 y, f32 width, f32 height, u32 tiledTextureIdx, u32 textureIdx);
inline u32                getTextureId(TextureModel textureModel)
{
  return g_renderer.textures[textureModel].textureId;
}

#endif
