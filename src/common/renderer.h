#ifndef RENDERER_H
#define RENDERER_H

#include "common.h"
#include "entity.h"
#include "font.h"
#include "map.h"
#include "texture.h"
#include "ui.h"

#define TEXT_MAX_LENGTH 32

enum TextureModel
{
  TEXTURE_TILES,
  TEXTURE_BACKGROUNDS,
  TEXTURE_CHARACTERS,
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

extern TextureTiled       g_tiledTextures[8];

struct Renderer
{
  SDL_Window*   window;
  SDL_GLContext context;
  GLuint        textureProgramId;
  GLuint        textureVertexId;
  GLuint        textureBufferId;
  GLuint        lineProgramId;
  GLuint        lineVertexId;
  GLuint        lineBufferId;
  Font*         font;
  Texture       textures[32];
  u32           textureCount;
};

typedef enum TextureModel TextureModel;
typedef struct Renderer   Renderer;
extern Renderer           g_renderer;

void                      renderUnfilledQuad(Vec2f32 start, Vec2f32 end, u32 width, Color* color);
void                      renderLine(Vec2f32 start, Vec2f32 end, u32 width, Color* color);
void                      updateWindowSize(i32 width, i32 height);
void                      renderComponent(UIComponent* comp);
void                      renderTexture(Matrix3x3* transMatrix, u32 textureIdx);
void                      renderButton(ButtonUIComponent* button);
void                      renderSlider(SliderUIComponent* slider);
void                      renderCheckbox(CheckboxUIComponent* checkbox);
void                      renderDropdown(DropdownUIComponent* dropdown);
void                      renderTextCentered(const char* text, Color* color, f32 x, f32 y, f32 fontSize, f32 spaceSize);
void                      renderTextEndsAt(const char* text, Color* color, f32 x, f32 y, f32 fontSize, f32 spaceSize);
void                      renderTextStartsAt(const char* text, Color* color, f32 x, f32 y, f32 fontSize, f32 spaceSize);
void                      renderTextEndsAt(const char* text, Color* color, f32 x, f32 y, f32 fontSize, f32 spaceSize);
void                      renderEntity(Entity* entity);
void                      renderMap(Map* map);
void                      renderHealth(u8 hp);
void                      initTiledTextures();
void                      renderTextureTile(f32 x, f32 y, f32 width, f32 height, u32 tiledTextureIdx, u32 textureIdx);
void                      rebindFullTexture();
void                      initRenderer(Font* font, const char* textureLocation);

inline TextureTiled*      getTiledTexture(TextureModel textureModel)
{
  return &g_tiledTextures[textureModel];
}

inline u32 getTextureId(TextureModel textureModel)
{
  return g_renderer.textures[textureModel].textureId;
}
Texture*           getTexture(u32 textureIdx);

static inline void initNewFrame(Color color)
{
  glClearColor(color.x, color.y, color.z, color.a);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

#endif
