#version 400 

in vec2 inputPosition;

void main(void)
{
  gl_Position = vec4(inputPosition.x, inputPosition.y,0.0f, 1.0f);
}
