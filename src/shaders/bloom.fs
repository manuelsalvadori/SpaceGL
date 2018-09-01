#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform sampler2D vignette;
uniform float vigAlpha;
uniform float exposure;
uniform bool bloom;

float FXAA_SPAN_MAX = 8.0f;
float FXAA_REDUCE_MUL = 1.0f/8.0f;
float FXAA_REDUCE_MIN = 1.0f/128.0f;

uniform float width;
uniform float height;
vec3 computeFxaa();

vec3 Fxaa(sampler2D sampler0, vec2 resolution)
{
	vec2 inverse_resolution=vec2(1.0/resolution.x,1.0/resolution.y);
	vec3 rgbNW = texture(sampler0, (gl_FragCoord.xy + vec2(-1.0,-1.0)) * inverse_resolution).xyz;
	vec3 rgbNE = texture(sampler0, (gl_FragCoord.xy + vec2(1.0,-1.0)) * inverse_resolution).xyz;
	vec3 rgbSW = texture(sampler0, (gl_FragCoord.xy + vec2(-1.0,1.0)) * inverse_resolution).xyz;
	vec3 rgbSE = texture(sampler0, (gl_FragCoord.xy + vec2(1.0,1.0)) * inverse_resolution).xyz;
	vec3 rgbM  = texture(sampler0,  gl_FragCoord.xy  * inverse_resolution).xyz;
	vec3 luma = vec3(0.299, 0.587, 0.114);
	float lumaNW = dot(rgbNW, luma);
	float lumaNE = dot(rgbNE, luma);
	float lumaSW = dot(rgbSW, luma);
	float lumaSE = dot(rgbSE, luma);
	float lumaM  = dot(rgbM,  luma);
	float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
	float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE))); 
	vec2 dir;
	dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
	dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));
	float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL),FXAA_REDUCE_MIN);
	float rcpDirMin = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);
	dir = min(vec2( FXAA_SPAN_MAX,  FXAA_SPAN_MAX),max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX),dir * rcpDirMin)) * inverse_resolution;
	vec3 rgbA = 0.5 * (texture(sampler0,   gl_FragCoord.xy  * inverse_resolution + dir * (1.0/3.0 - 0.5)).xyz + texture(sampler0,   gl_FragCoord.xy  * inverse_resolution + dir * (2.0/3.0 - 0.5)).xyz);
	vec3 rgbB = rgbA * 0.5 + 0.25 * (texture(sampler0,  gl_FragCoord.xy  * inverse_resolution + dir *  - 0.5).xyz + texture(sampler0,  gl_FragCoord.xy  * inverse_resolution + dir * 0.5).xyz);
	float lumaB = dot(rgbB, luma);
	
	if((lumaB < lumaMin) || (lumaB > lumaMax))
		return rgbA;
	else
		return rgbB;
}

void main()
{             
	const float gamma = 2.2;
	//vec3 hdrColor = texture(scene, TexCoords).rgb;
	vec3 hdrColor = Fxaa(scene, vec2(width, height));
	vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
	vec4 vignetteColor = texture(vignette, TexCoords);
	
	if(bloom)
		hdrColor += bloomColor; // additive blending
	
	// tone mapping
	//hdrColor = vec3(1.0) - exp(-hdrColor * exposure);
	// also gamma correct while we're at it       
	//hdrColor = pow(hdrColor, vec3(1.0 / gamma));

	FragColor = vec4(hdrColor, 1.0) + vignetteColor*vigAlpha;
}