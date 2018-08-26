#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform float exposure;
uniform bool bloom;

void main()
{             
    const float gamma = 2.2;
    vec3 hdrColor = texture(scene, TexCoords).rgb;      
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    
    // tone mapping
    //bloomColor = vec3(1.0) - exp(-bloomColor * exposure);
    // also gamma correct while we're at it       
    //bloomColor = pow(bloomColor, vec3(1.0 / gamma));
    
    if(bloom)
    		hdrColor += bloomColor; // additive blending
    
    FragColor = vec4(hdrColor, 1.0);
}