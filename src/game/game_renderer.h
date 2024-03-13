#ifndef GAME_RENDERER_H
#define GAME_RENDERER_H

#include "entity.h"
#include "renderer.h"

enum TextureModel
{
  TEXTURE_PLAYER_MODEL,
  TEXTURE_GREY_BUTTON_05,
  TEXTURE_GREY_BOX,
  TEXTURE_GREY_CHECKMARK_GREY,
  TEXTURE_GREY_SLIDER_UP,
  TEXTURE_GREY_SLIDER_HORIZONTAL,
  TEXTURE_GREY_BUTTON_14,
  TEXTURE_FONT
};
typedef enum TextureModel TextureModel;

void                      renderEntity(Entity* entity);
void                      renderHealth(u8 hp);
inline u32                getTextureId(TextureModel textureModel)
{
  return g_renderer.textures[textureModel].textureId;
}

#endif
