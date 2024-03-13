#ifndef RENDERER_H
#define RENDERER_H

#include "common.h"
#include "font.h"
#include "texture.h"
#include "ui.h"

#define TEXT_MAX_LENGTH 32

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
Texture*                  getTexture(u32 textureIdx);

void                      initRenderer(Font* font, const char* textureLocation);
static inline void        initNewFrame(Color color)
{
  glClearColor(color.x, color.y, color.z, color.a);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

#endif
