
#version 330 core
layout (location = 0) in vec3 aPos; //layout location indica l'indice nel VAO
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMat;


void main()
{
	FragPos = vec3(model * vec4(aPos, 1.0));
    TexCoords = aTexCoords;
    Normal = normalMat * aNormal;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
