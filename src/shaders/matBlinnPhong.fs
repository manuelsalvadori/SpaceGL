#version 330 core
out vec4 FragColor;

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

uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform sampler2D texture_diffuse1;
uniform sampler2D shadowMap;
uniform bool falcon;

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
	// check whether current frag pos is in shadow
	// float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
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
	vec3 color = texture(texture_diffuse1, TexCoords).rgb;

	// ambient
	vec3 ambient = 0.5 * color * light.ambient * material.ambient;

	// diffuse
	vec3 lightDir = normalize(light.position - FragPos);
	vec3 normal = normalize(Normal);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = light.diffuse * (diff * color * material.diffuse);//diff * color;

	// specular
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfwayDir = normalize(lightDir + viewDir);  
	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess) * when_gt(diff, 0.0f);

	vec3 specular = light.specular * (spec * material.specular);  //vec3(0.3) * spec; // assuming bright white light color
	//FragColor = vec4(ambient + diffuse + specular, 1.0);
	// calculate shadow
	float shadow = ShadowCalculation(FragPosLightSpace);                    
	vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
	if(falcon) lighting = ambient + diffuse + specular;
	FragColor = vec4(lighting, 1.0);
	

}