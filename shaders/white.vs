#version 400

in vec3 inputPosition;

void main(void)
{
  gl_Position = vec4(inputPosition, 1.0f);
}
