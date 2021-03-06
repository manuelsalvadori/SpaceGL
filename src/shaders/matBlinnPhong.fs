#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;    
	float shininess;
}; 

struct Light
{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;
in vec4 FragPosLightSpace;
in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_emission1;
uniform sampler2D shadowMap;
uniform float time;

// visual debug bounding sphere
//in vec3 debugOrigin;

float when_gt(float x, float y)
{
	return max(sign(x - y), 0.0);
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
	// perform perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;
	// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
	float closestDepth = texture(shadowMap, projCoords.xy).r; 
	// get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;
	// calculate bias (based on depth map resolution and slope)
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
	// PCF
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
		}    
	}
	shadow /= 9.0;

	// keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
	if(projCoords.z > 1.0)
		shadow = 0.0;

	return shadow;
}

void main()
{
	// obtain normal from normal map in range [0,1]
	vec3 normal = texture(texture_normal1, TexCoords).rgb;
	// transform normal vector to range [-1,1]
	normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space

	vec3 color = texture(texture_diffuse1, TexCoords).rgb * 1.5f;
	vec3 emission = texture(texture_emission1, TexCoords).rgb;

	// ambient
	vec3 ambient = 0.5 * color * light.ambient * material.ambient;

	// diffuse
	vec3 lightDir = normalize(TangentLightPos - TangentFragPos);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = light.diffuse * (diff * color * material.diffuse);//diff * color;

	// specular
	vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfwayDir = normalize(lightDir + viewDir);  
	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess) * when_gt(diff, 0.0f);

	vec3 specular = light.specular * spec * texture(texture_specular1, TexCoords).rgb*10.0f;    //vec3(0.3) * spec; // assuming bright white light color
	//FragColor = vec4(ambient + diffuse + specular, 1.0); // no shadows

	// calculate shadow
	float shadow = ShadowCalculation(FragPosLightSpace);
	
	vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));// + emission;
	// bloom
	if(dot(emission, vec3(0.299, 0.587, 0.114)) > 0.2)
	{
		lighting += emission;
		BrightColor = vec4(lighting * (vec3(1.f,1.2f,1.5f)*1.66f), 0.9); //((sin(time*10.f)+1.f)/3.f)+1.f)
		lighting -= emission;
		lighting += vec3(0.7,1.0,1.0);
	}
	else
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
	
	FragColor = vec4(lighting, 1.0);
	// visual debug bounding sphere
//	if(abs(length(debugOrigin - FragPos)) < 2)
//		FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}