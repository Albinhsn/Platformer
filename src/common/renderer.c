#include "renderer.h"
#include "common.h"
#include "files.h"
#include "font.h"
#include <GL/glext.h>
#include <GL/gl.h>
#include "sdl.h"
#include "texture.h"
#include "vector.h"
#include <string.h>

Renderer g_renderer;

void     updateWindowSize(i32 width, i32 height)
{
  updateWindowSizeSDL(g_renderer.window, width, height);
}

void generateTextures(const char* textureLocations)
{
  //glActiveTexture(GL_TEXTURE0);
  FILE* filePtr = fopen(textureLocations, "rb");
  if (filePtr == 0)
  {
    printf("Failed to open stateVariables '%s'\n", textureLocations);
    return;
  }
  char buffer[256];
  memset(buffer, 0, 256);
  u8 idx = 0;
  while (fgets(buffer, 256, filePtr))
  {
    Texture* texture = &g_renderer.textures[idx];
    glGenTextures(1, &texture->textureId);
    glBindTexture(GL_TEXTURE_2D, texture->textureId);

    buffer[strlen(buffer) - 1] = '\0';
    bool result                = parsePNG(&texture->data, &texture->width, &texture->height, buffer);
    if (!result)
    {
      continue;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    sta_glGenerateMipmap(GL_TEXTURE_2D);
    idx++;
  }
}

void createLineVertexArray()
{
  GLfloat bufferData[4] = {0.0f, 0.0f, 0.0f, 0.0f};

  i32     indices[]     = {0, 1};

  sta_glGenVertexArrays(1, &g_renderer.lineVertexId);
  sta_glBindVertexArray(g_renderer.lineVertexId);

  sta_glGenBuffers(1, &g_renderer.lineBufferId);
  sta_glBindBuffer(GL_ARRAY_BUFFER, g_renderer.lineBufferId);

  sta_glEnableVertexAttribArray(0);

  sta_glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, 0);

  u32 indexBufferId;
  sta_glGenBuffers(1, &indexBufferId);
  sta_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
  sta_glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * 2, indices, GL_STATIC_DRAW);

  sta_glBindVertexArray(0);
}

void createTextureVertexArray()
{
  GLfloat bufferData[20] = {
      -1.0f, -1.0f, 0.0f, 1.0f, //
      1.0f,  -1.0f, 1.0f, 1.0f, //
      -1.0f, 1.0f,  0.0f, 0.0f, //
      1.0f,  1.0f,  1.0f, 0.0f  //
  };
  int    indices[6] = {0, 1, 2, 1, 3, 2};

  GLuint indexBufferId;

  sta_glGenVertexArrays(1, &g_renderer.textureVertexId);
  sta_glBindVertexArray(g_renderer.textureVertexId);

  sta_glGenBuffers(1, &g_renderer.textureBufferId);
  sta_glBindBuffer(GL_ARRAY_BUFFER, g_renderer.textureBufferId);
  sta_glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(GLfloat), bufferData, GL_STATIC_DRAW);

  sta_glEnableVertexAttribArray(0);
  sta_glEnableVertexAttribArray(1);

  sta_glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, 0);
  sta_glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (signed char*)NULL + (2 * sizeof(GLfloat)));

  sta_glGenBuffers(1, &indexBufferId);
  sta_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
  sta_glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), indices, GL_STATIC_DRAW);

  sta_glBindVertexArray(0);
}

void createAndCompileShader(GLuint* shaderId, int glShaderMacro, const char* source)
{
  char* buffer;
  int   len;
  readFile(&buffer, &len, source);
  *shaderId = sta_glCreateShader(glShaderMacro);
  sta_glShaderSource(*shaderId, 1, (const char**)&buffer, NULL);

  sta_glCompileShader(*shaderId);

  int result;
  sta_glGetShaderiv(*shaderId, GL_COMPILE_STATUS, &result);
  if (result != 1)
  {
    int logSize;

    sta_glGetShaderiv(*shaderId, GL_INFO_LOG_LENGTH, &logSize);
    logSize++;
    char infoLog[logSize];
    infoLog[logSize - 1] = '\0';

    sta_glGetShaderInfoLog(*shaderId, logSize, NULL, infoLog);
    printf("Failed to compile shader\n");
    printf("%s\n", infoLog);
    exit(1);
  }

  free(buffer);
}

void createAndCompileVertexShader(GLuint* shaderId, const char* source)
{
  createAndCompileShader(shaderId, GL_VERTEX_SHADER, source);
}

void createAndCompileFragmentShader(GLuint* shaderId, const char* source)
{
  createAndCompileShader(shaderId, GL_FRAGMENT_SHADER, source);
}

void createTextShaderProgram()
{
  GLuint vShader, fShader;
  createAndCompileVertexShader(&vShader, "shaders/font.vs");
  createAndCompileFragmentShader(&fShader, "shaders/font.ps");

  Font* font      = g_renderer.font;
  font->programId = sta_glCreateProgram();
  sta_glAttachShader(font->programId, vShader);
  sta_glAttachShader(font->programId, fShader);

  sta_glBindAttribLocation(font->programId, 0, "inputPosition");
  sta_glBindAttribLocation(font->programId, 1, "inputTexCoord");

  sta_glLinkProgram(font->programId);
}

void createLineShaderProgram()
{
  GLuint vShader, fShader;
  createAndCompileVertexShader(&vShader, "shaders/line.vs");
  createAndCompileFragmentShader(&fShader, "shaders/line.ps");

  g_renderer.lineProgramId = sta_glCreateProgram();
  sta_glAttachShader(g_renderer.lineProgramId, vShader);
  sta_glAttachShader(g_renderer.lineProgramId, fShader);

  sta_glBindAttribLocation(g_renderer.lineProgramId, 0, "inputPosition");

  sta_glLinkProgram(g_renderer.lineProgramId);
}

void createTextureShaderProgram()
{
  GLuint vShader, fShader;
  createAndCompileVertexShader(&vShader, "shaders/texture.vs");
  createAndCompileFragmentShader(&fShader, "shaders/texture.ps");

  g_renderer.textureProgramId = sta_glCreateProgram();
  sta_glAttachShader(g_renderer.textureProgramId, vShader);
  sta_glAttachShader(g_renderer.textureProgramId, fShader);

  sta_glBindAttribLocation(g_renderer.textureProgramId, 0, "inputPosition");
  sta_glBindAttribLocation(g_renderer.textureProgramId, 1, "inputTexCoord");

  sta_glLinkProgram(g_renderer.textureProgramId);
}

void initRenderer(Font* font, const char* textureLocation)
{
  g_renderer.window = initSDLWindow(&g_renderer.context, DEFAULT_SCREENWIDTH, DEFAULT_SCREENHEIGHT);
  g_renderer.font   = font;

  generateTextures(textureLocation);
  createTextShaderProgram();
  initFont(font);

  createTextureShaderProgram();
  createTextureVertexArray();

  createLineShaderProgram();
  createLineVertexArray();
}


void renderTexture(Matrix3x3* transMatrix, u32 textureIdx)
{
  sta_glUseProgram(g_renderer.textureProgramId);
  sta_glBindVertexArray(g_renderer.textureVertexId);

  glBindTexture(GL_TEXTURE_2D, textureIdx);

  i32 location = sta_glGetUniformLocation(g_renderer.textureProgramId, "transMatrix");
  if (location == -1)
  {
    printf("failed to set transMatrix\n");
    exit(1);
  }
  sta_glUniformMatrix3fv(location, 1, true, (f32*)transMatrix);

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  sta_glBindVertexArray(0);
}

static void setLineShaderParams(Color* color)
{
  sta_glUseProgram(g_renderer.lineProgramId);

  int location = sta_glGetUniformLocation(g_renderer.lineProgramId, "pixelColor");
  if (location == -1)
  {
    printf("Failed to get pixel color location\n");
    exit(1);
  }
  f32 c[4] = {color->r, color->g, color->b, color->a};
  sta_glUniform4fv(location, 1, &c[0]);
}

static void setTextShaderParams(Font* font, Color* color)
{
  sta_glUseProgram(font->programId);

  i32 location = sta_glGetUniformLocation(font->programId, "fontTexture");
  if (location == -1)
  {
    printf("Failed to get fontTexture location\n");
    exit(1);
  }
  sta_glUniform1i(location, 0);

  location = sta_glGetUniformLocation(font->programId, "pixelColor");
  if (location == -1)
  {
    printf("Failed to get pixel color location\n");
    exit(1);
  }
  f32 c[4] = {color->r, color->g, color->b, color->a};
  sta_glUniform4fv(location, 1, &c[0]);
}

void renderUnfilledQuad(Vec2f32 start, Vec2f32 end, u32 width, Color* color)
{

  renderLine(CREATE_VEC2f32(start.x, start.y), CREATE_VEC2f32(end.x, start.y), width, color);
  renderLine(CREATE_VEC2f32(start.x, start.y), CREATE_VEC2f32(start.x, end.y), width, color);
  renderLine(CREATE_VEC2f32(end.x, start.y), CREATE_VEC2f32(end.x, end.y), width, color);
  renderLine(CREATE_VEC2f32(start.x, end.y), CREATE_VEC2f32(end.x, end.y), width, color);
}

void renderLine(Vec2f32 start, Vec2f32 end, u32 width, Color* color)
{
  setLineShaderParams(color);
  sta_glBindVertexArray(g_renderer.lineVertexId);

  sta_glBindBuffer(GL_ARRAY_BUFFER, g_renderer.lineBufferId);
  f32 bufferData[4] = {
      start.x * 0.01f, start.y * 0.01f, //
      end.x * 0.01f, end.y * 0.01f      //
  };

  sta_glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4, bufferData, GL_STATIC_DRAW);
  glLineWidth(width);
  glDrawElements(GL_LINE_STRIP, 2, GL_UNSIGNED_INT, 0);
  glLineWidth(1);
}

static void renderText(Font* font, Color* color)
{
  setTextShaderParams(font, color);
  sta_glBindVertexArray(font->vertexArrayId);
  glBindTexture(GL_TEXTURE_2D, font->textureId);

  glDrawElements(GL_TRIANGLES, TEXT_MAX_LENGTH * 4, GL_UNSIGNED_INT, 0);
}
void renderTextCentered(const char* text, Color* color, f32 x, f32 y, f32 fontSize, f32 spaceSize)
{
  Font* font = g_renderer.font;
  updateText(font, text, x, y, fontSize, spaceSize, TEXT_INDENTATION_CENTERED);
  renderText(font, color);
}

void renderTextStartsAt(const char* text, Color* color, f32 x, f32 y, f32 fontSize, f32 spaceSize)
{
  Font* font = g_renderer.font;
  updateText(font, text, x, y, fontSize, spaceSize, TEXT_INDENTATION_START);
  renderText(font, color);
}

void renderTextEndsAt(const char* text, Color* color, f32 x, f32 y, f32 fontSize, f32 spaceSize)
{
  Font* font = g_renderer.font;
  updateText(font, text, x, y, fontSize, spaceSize, TEXT_INDENTATION_END);
  renderText(font, color);
}

void renderComponent(UIComponent* comp)
{
  Matrix3x3 transMatrix;
  clearMat3x3(&transMatrix);
  getTransformationMatrix(&transMatrix, comp->x, comp->y, comp->width, comp->height);
  Texture texture = g_renderer.textures[comp->textureIdx];
  renderTexture(&transMatrix, texture.textureId);
}

void renderButton(ButtonUIComponent* button)
{
  renderComponent(&button->component);
  renderTextCentered(button->text, &button->color, button->component.x, button->component.y, button->fontSize, button->spaceSize);
}
void renderSlider(SliderUIComponent* slider)
{
  renderComponent(&slider->background);
  renderComponent(&slider->bar);
  renderComponent(&slider->slider);
}

void renderCheckbox(CheckboxUIComponent* checkbox)
{
  renderComponent(&checkbox->background);
  if (checkbox->toggled)
  {
    renderComponent(&checkbox->checkmark);
  }
}

void renderDropdown(DropdownUIComponent* dropdown)
{
  renderButton(&dropdown->dropdownButton);
  if (dropdown->toggled)
  {
    for (u32 i = 0; i < dropdown->itemCount; i++)
    {
      renderButton(&dropdown->items[i]);
    }
  }
}
