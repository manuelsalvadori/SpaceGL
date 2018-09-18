#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

uniform vec4 m_MainColor;
uniform float g_Time;

uniform sampler2D texture_diffuse1;

uniform float m_BarSpeed;
uniform float m_BarDistance;
uniform float m_Alpha;
uniform float m_FlickerSpeed;
uniform vec4 m_RimColor;
uniform float m_RimPower;
uniform float m_GlowSpeed;
uniform float m_GlowDistance;

in vec3 vertexWorldPos;
in vec3 vertexModelPos;
in vec3 worldNormal;
in vec3 viewDir;
in vec2 texCoord;

float rand(float n)
{
	return fract(sin(n) * 43758.5453123);
}

float noise(float p)
{
	return mix(rand(floor(p)), rand(floor(p) + 1.0), fract(p));
}

void main()
{

	//Scan effect
	float bars = 0.0;
	float val = g_Time * m_BarSpeed + vertexWorldPos.y * m_BarDistance;
	bars = smoothstep(val - floor(val),0.2, 0.5) * 0.65;
	
	val = g_Time * m_BarSpeed + vertexWorldPos.x * m_BarDistance* 0.5;
	bars += smoothstep(val - floor(val), 0.2, 0.5) * 0.1;

	//Flickering
	float flicker = 1.0;
	flicker = clamp(noise(g_Time * m_FlickerSpeed * 100), 0.165, 1.0);

	//Rim lighting
	float rim = 1.0;
	vec4 rimColor = vec4(0.0);
	rim = 1.0 - clamp(dot(viewDir, worldNormal), 0.0, 1.0);
	rimColor = m_RimColor * pow(rim, m_RimPower);

	//Glow
	float glow = 0.0;
	float tempGlow = vertexWorldPos.y * m_GlowDistance - g_Time * m_GlowSpeed;
	glow = fract(tempGlow) * 0.6;

	vec4 color = m_MainColor + rimColor + (glow * 0.35 * m_MainColor);
	color.a = m_Alpha * (bars + rim + glow) * flicker;
	FragColor = color;
	BrightColor = vec4(color.xyz*0.3, 1.0);
}