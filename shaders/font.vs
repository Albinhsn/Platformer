#version 400


in vec3 inputPosition;
in vec2 inputTexCoord;


out vec2 texCoord;

void main(void)
{
	gl_Position = vec4(inputPosition, 1.0f);

	texCoord = inputTexCoord;
}
