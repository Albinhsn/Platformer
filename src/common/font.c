#include "font.h"
#include "renderer.h"

static void parseFontTypes(struct Font* font, const char* fileLocation)
{

  char* buffer;
  int   len;
  readFile(&buffer, &len, fileLocation);
  FILE* file;
  file = fopen(fileLocation, "r");
  char line[256];
  char number[32];
  for (int i = 0; i < 95; i++)
  {
    memset(number, 0, 32);
    fgets(line, sizeof(line), file);
    u8 pos = 0;
    u8 inc = 0;
    // remove number
    i32 idx;
    parseIntFromString(&idx, line, &inc);
    pos = inc;

    // remove whitespace, number and another whitespace
    pos += 3;
    parseFloatFromString(&font->type[idx].left, &line[pos], &inc);
    pos += inc;

    // This should always be the start of the number
    // parse whitespace again
    while (line[pos] == ' ')
    {
      pos++;
    }

    parseFloatFromString(&font->type[idx].right, &line[pos], &inc);
    pos += inc;
    while (line[pos] == ' ')
    {
      pos++;
    }
    parseIntFromString(&font->type[idx].size, &line[pos], &inc);
  }
}

static void createTextBuffers(Font* font)
{
  GLuint indexBufferId;

  int    vertexCount = TEXT_MAX_LENGTH * 4;
  GLuint indices[TEXT_MAX_LENGTH * 6];
  for (int i = 0, idx = 0; idx < TEXT_MAX_LENGTH * 6; idx += 6, i += 4)
  {
    indices[idx + 0] = i + 0;
    indices[idx + 1] = i + 1;
    indices[idx + 2] = i + 2;
    indices[idx + 3] = i + 0;
    indices[idx + 4] = i + 3;
    indices[idx + 5] = i + 1;
  }

  GLfloat vertices[vertexCount];
  for (int i = 0; i < vertexCount; i++)
  {
    vertices[i] = 0;
  }

  sta_glGenVertexArrays(1, &font->vertexArrayId);
  sta_glBindVertexArray(font->vertexArrayId);

  sta_glGenBuffers(1, &font->vertexBufferId);
  sta_glBindBuffer(GL_ARRAY_BUFFER, font->vertexBufferId);
  sta_glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(GLfloat), vertices, GL_DYNAMIC_DRAW);

  sta_glEnableVertexAttribArray(0);
  sta_glEnableVertexAttribArray(1);
  sta_glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(GLfloat) * 5, 0);
  sta_glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(GLfloat) * 5, (unsigned char*)NULL + (3 * sizeof(float)));

  sta_glGenBuffers(1, &indexBufferId);

  sta_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
  sta_glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

  sta_glBindVertexArray(0);
}

void initFont(Font* font)
{
  parseFontTypes(font, FONT_DATA_LOCATION);
  createTextBuffers(font);
  font->textureId = getTextureId(TEXTURE_FONT);
}

void buildUpdatedTextVertexArray(Font* font, f32* vertices, u32 vertexCount, const char* text, f32 x, f32 y, f32 spaceSize, f32 fontSize, TextIndentation indentation)
{
  f32 drawX        = x * 0.01f;
  f32 drawY        = (y + fontSize) * 0.01f;
  f32 height       = fontSize * 0.04f;

  f32 sizeModifier = 0.5f;

  i32 numLetters   = strlen(text);
  if (numLetters > TEXT_MAX_LENGTH)
  {
    printf("WARNING: Trying to write text with %d characters, %d is max", numLetters, TEXT_MAX_LENGTH);
    numLetters = TEXT_MAX_LENGTH;
  }

  f32 totalSize = 0;
  for (u32 i = 0; i < numLetters; i++)
  {
    char letter    = text[i];
    f32  addedSize = font->type[(u8)letter].size * 0.01f * sizeModifier;
    totalSize += addedSize != 0 ? addedSize : spaceSize * 0.01f;
  }
  if (indentation == TEXT_INDENTATION_CENTERED)
  {
    drawX -= totalSize / 2.0f;
  }
  else if (indentation == TEXT_INDENTATION_END)
  {
    drawX -= totalSize;
  }

  for (i32 letterIdx = 0, vertexIdx = 0; letterIdx < numLetters; letterIdx++)
  {
    char letter = text[letterIdx];
    if (letter == 32)
    {
      drawX += spaceSize * 0.01f;
    }
    else
    {
      FontType type           = font->type[(u8)letter];
      f32      size           = type.size * 0.01f * sizeModifier;

      vertices[vertexIdx + 0] = drawX;
      vertices[vertexIdx + 1] = drawY;
      vertices[vertexIdx + 2] = 0.0f;
      vertices[vertexIdx + 3] = type.left;
      vertices[vertexIdx + 4] = 0.0f;
      vertexIdx += 5;

      vertices[vertexIdx + 0] = drawX + size;
      vertices[vertexIdx + 1] = drawY - height;
      vertices[vertexIdx + 2] = 0.0f;
      vertices[vertexIdx + 3] = type.right;
      vertices[vertexIdx + 4] = 1.0f;
      vertexIdx += 5;

      vertices[vertexIdx + 0] = drawX;
      vertices[vertexIdx + 1] = drawY - height;
      vertices[vertexIdx + 2] = 0.0f;
      vertices[vertexIdx + 3] = type.left;
      vertices[vertexIdx + 4] = 1.0f;
      vertexIdx += 5;

      vertices[vertexIdx + 0] = drawX + size;
      vertices[vertexIdx + 1] = drawY;
      vertices[vertexIdx + 2] = 0.0f;
      vertices[vertexIdx + 3] = type.right;
      vertices[vertexIdx + 4] = 0.0f;
      vertexIdx += 5;

      drawX += size + 0.002f;
    }
  }
}

void updateText(Font* font, const char* text, f32 x, f32 y, f32 fontSize, f32 spaceSize, TextIndentation indentation)
{
  u32 vertexCount = TEXT_MAX_LENGTH * 4 * 5;
  f32 vertices[vertexCount];
  for (int i = 0; i < vertexCount; i++)
  {
    vertices[i] = 0.0f;
  }

  sta_glBindVertexArray(font->vertexArrayId);

  buildUpdatedTextVertexArray(font, vertices, vertexCount, text, x, y, spaceSize, fontSize, indentation);

  sta_glBindBuffer(GL_ARRAY_BUFFER, font->vertexArrayId);
  sta_glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(f32), vertices, GL_DYNAMIC_DRAW);

  sta_glBindVertexArray(0);
}
