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

uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform sampler2D texture_diffuse1;

float when_gt(float x, float y)
{
  return max(sign(x - y), 0.0);
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
	FragColor = vec4(ambient + diffuse + specular, 1.0);
}