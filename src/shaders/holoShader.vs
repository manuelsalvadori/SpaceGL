#version 330 core
layout (location = 0) in vec3 aPos; //layout location indica l'indice nel VAO
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 vertexWorldPos;
out vec3 vertexModelPos;
out vec3 worldNormal;
out vec3 viewDir;
out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMat;

uniform float g_Time;
uniform float m_GlitchSpeed;
uniform float m_GlitchIntensity;

void main()
{
	mat4 viewModel = view * model;
	vertexWorldPos = (model * vec4(aPos, 1.0)).xyz;
	vertexModelPos = aPos;
	texCoord = aTexCoords;
	worldNormal = normalize(normalMat * aNormal);
	viewDir = normalize(-(viewModel * vec4(aPos, 1.0)).xyz);

	vec3 outPosition = (model * vec4(aPos, 1.0)).xyz;
	
	//Glitch vertex displacement
	outPosition.x += sign(sin(g_Time)) * m_GlitchIntensity * step(0.5, sin(g_Time * 2.0 + aPos.y)) * step(0.99, sin(g_Time * m_GlitchSpeed*1.1 * 0.5));

	gl_Position = (projection * view) * vec4(outPosition, 1.0);
}