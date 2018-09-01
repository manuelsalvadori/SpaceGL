#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec3 FragPos;
in vec2 TexCoords;

void main()
{
	FragColor = vec4(1.0, 1.0, 1.0, 1.0);
	BrightColor = vec4(0.96, 0.0, 0.0, 0.91);
}