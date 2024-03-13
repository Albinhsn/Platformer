#ifndef FONT_H
#define FONT_H

#define TEXT_MAX_LENGTH 32
#include "sdl.h"
#include "texture.h"
#include <ctype.h>

struct FontType
{
  float left, right;
  int   size;
};
typedef struct FontType FontType;

struct Font
{
  GLuint   programId;
  GLuint   vertexArrayId;
  GLuint   vertexBufferId;
  GLuint   textureId;
  FontType type[256];
};
typedef struct Font Font;

enum TextIndentation
{
  TEXT_INDENTATION_START,
  TEXT_INDENTATION_CENTERED,
  TEXT_INDENTATION_END,
};
typedef enum TextIndentation TextIndentation;

void initFont(Font* font);
void buildUpdatedTextVertexArray(Font* font, f32* vertices, u32 vertexCount, const char* text, f32 x, f32 y, f32 spaceSize, f32 fontSize, TextIndentation indentation);
void updateText(Font* font, const char* text, f32 x, f32 y, f32 fontSize, f32 spaceSize, TextIndentation indentation);

#endif
