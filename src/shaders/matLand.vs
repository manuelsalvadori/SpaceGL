
#version 330 core
layout (location = 0) in vec3 aPos; //layout location indica l'indice nel VAO
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;
out vec4 FragPosLightSpace;

out vec3 TangentLightPos;
out vec3 TangentViewPos;
out vec3 TangentFragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMat;
uniform mat4 lightSpaceMatrix;

uniform float time;

uniform vec3 lightPos;
uniform vec3 viewPos;


void main()
{
	FragPos = vec3(model * vec4(aPos, 1.0));
	
	//TexCoords = aTexCoords;
	TexCoords = vec2(aTexCoords.x, aTexCoords.y+(time/3.0f));
	Normal = normalMat * aNormal;

	vec3 T = normalize(normalMat * aTangent);
	vec3 N = normalize(normalMat * aNormal);
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);

	mat3 TBN = transpose(mat3(T, B, N));    
	TangentLightPos = TBN * lightPos;
	TangentViewPos  = TBN * viewPos;
	TangentFragPos  = TBN * FragPos;


	FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);

	gl_Position = projection * view * vec4(FragPos, 1.0);
}
