#include "editor_renderer.h"
#include "sdl.h"

TextureTiled g_tiledTextures[8];

void         renderTextureTile(f32 x, f32 y, f32 width, f32 height, u32 tiledTextureIdx, u32 textureIdx)
{

  Matrix3x3 transMatrix;
  clearMat3x3(&transMatrix);
  getTransformationMatrix(&transMatrix, x, y, width, height);

  // Figure out which row and column it is
  TextureTiled texture = g_tiledTextures[tiledTextureIdx];
  u32          maxRow  = texture.texture->height / texture.dim;
  u32          maxCol  = texture.texture->width / texture.dim;
  u32          row     = textureIdx / maxCol;
  u32          col     = textureIdx % maxCol;

  if (row >= maxRow)
  {
    printf("SEVERE: Trying to access outside of texture\n");
    return;
  }

  f32     uvHeight       = 1.0f / (f32)maxRow;
  f32     uvWidth        = 1.0f / (f32)maxCol;

  f32     uvY            = uvHeight * row;
  f32     uvX            = uvWidth * col;

  GLfloat bufferData[20] = {
      -1.0f, -1.0f, uvX,           uvY + uvHeight, //
      1.0f,  -1.0f, uvX + uvWidth, uvY + uvHeight, //
      -1.0f, 1.0f,  uvX,           uvY,            //
      1.0f,  1.0f,  uvX + uvWidth, uvY             //
  };

  sta_glBindVertexArray(g_renderer.textureVertexId);
  sta_glBindBuffer(GL_ARRAY_BUFFER, g_renderer.textureBufferId);
  sta_glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(GLfloat), bufferData, GL_STATIC_DRAW);

  renderTexture(&transMatrix, texture.texture->textureId);
}

void initTiledTextures()
{
  const u32    numberOfTiles     = 3;
  TextureModel tiles[]           = {TEXTURE_TILES, TEXTURE_BACKGROUNDS, TEXTURE_CHARACTERS};
  u32          tileCountPerRow[] = {20, 4, 9};
  for (u32 i = 0; i < numberOfTiles; i++)
  {
    TextureTiled* tile = &g_tiledTextures[i];
    tile->texture      = &g_renderer.textures[tiles[i]];
    tile->dim          = tile->texture->width / tileCountPerRow[i];
  }
}
