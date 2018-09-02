#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec3 FragPos;
in vec2 TexCoords;

uniform sampler2D explosion;


void main()
{
	FragColor = texture(explosion, TexCoords).rgba;
	BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}