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

//thanks, github
float rand(float n){
	return fract(sin(n) * 43758.5453123);
}

float noise(float p){
	float fl = floor(p);
	float fc = fract(p);
	return mix(rand(fl), rand(fl + 1.0), fc);
}

void main(){

	//Texture
//	vec4 texColor = vec4(1.0);
//	texColor = texture2D(texture_diffuse1, texCoord);

	//Scan effect
	float bars = 0.0;
	float val = g_Time * m_BarSpeed + vertexWorldPos.y * m_BarDistance;
	bars = step(val - floor(val), 0.5) * 0.65;

	//Just plain old alpha
	float alpha = 1.0;
	alpha = m_Alpha;

	//Flickering
	float flicker = 1.0;
	flicker = clamp(noise(g_Time * m_FlickerSpeed), 0.65, 1.0);

	//Rim lighting
	float rim = 1.0;
	vec4 rimColor = vec4(0.0);
	rim = 1.0 - clamp(dot(viewDir, worldNormal), 0.0, 1.0);
	rimColor = m_RimColor * pow(rim, m_RimPower);

	//Glow
	float glow = 0.0;
	float tempGlow = vertexWorldPos.y * m_GlowDistance - g_Time * m_GlowSpeed;
	glow = tempGlow - floor(tempGlow);


	vec4 color = /*texColor **/ m_MainColor + rimColor + (glow * 0.35 * m_MainColor);
	color.a = /*texColor.a **/ alpha * (bars + rim + glow) * flicker;
	FragColor = color;
	BrightColor = vec4(color.xyz*0.3, 1.0);
	//BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}