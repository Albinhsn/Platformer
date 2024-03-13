#include "game_renderer.h"
#include "entity.h"
#include "renderer.h"
#include "vector.h"


void renderEntity(Entity* entity)
{
  Matrix3x3 transMatrix;
  clearMat3x3(&transMatrix);
  getTransformationMatrix(&transMatrix, entity->x, entity->y, entity->width, entity->height);
  Texture texture = g_renderer.textures[entity->textureIdx];
  renderTexture(&transMatrix, texture.textureId);
}
