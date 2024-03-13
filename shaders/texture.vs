#version 400 

in vec2 inputPosition;
in vec2 inputTexCoord;

out vec2 texCoord;

uniform mat3 transMatrix;

void main(void)
{
  vec3 pos = vec3(inputPosition, 1.0f) * transMatrix;

  gl_Position = vec4(pos.x, pos.y,0.0f, 1.0f);

  texCoord = inputTexCoord;
}
