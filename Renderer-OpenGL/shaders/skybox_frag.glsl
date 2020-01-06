#version 420 core
out vec4 FragColor;

in VS_OUT {
    vec3 TexCoords;
	vec3 WorldPos;
} fs_in;

uniform samplerCube skybox;


void main()
{
//	vec3 up_colour = vec3(0.5, 0.5, 0.8);
//	vec3 down_colour = vec3(0.3, 0.5, 0.3);
//
//	// Only works cause the cube is normailzed
//	float t = clamp(fs_in.TexCoords.y, 0 ,1); 
//
//	float bias = 0.6f;	
//	float up_colour_amount = clamp((t + bias), 0.f, 1.f); 
//	float down_colour_amount = (1 - t);
//	vec3 colour = up_colour * up_colour_amount + down_colour_amount * down_colour;
//
    vec3 envColor = texture(skybox, fs_in.WorldPos).rgb;
    
    // HDR tonemap and gamma correct
    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2)); 
    
    FragColor = vec4(envColor, 1.0);

//	FragColor = vec4(colour, 1);
//	FragColor = texture(skybox, fs_in.TexCoords);
//	FragColor = vec4(fs_in.TexCoords,1);
}










