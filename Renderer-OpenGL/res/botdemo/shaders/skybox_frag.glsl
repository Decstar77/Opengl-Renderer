#version 330 core
layout (location = 0) out vec4 FragColour;

in vec3 world_position;

uniform bool has_skybox;
uniform samplerCube skybox;

void main()
{	
	vec3 colour = vec3(0);
	// @NOTE: Static-flow contorl
	if (has_skybox) 
	{	
		colour = texture(skybox, world_position).rgb;		
	}
	else
	{
		vec3 dir  = normalize(world_position);
		vec3 up  = vec3(0, 1, 0);
		float value =  dot(dir, up);
		float t = (value + 1) /2.f;
		
		//vec3 colour_a = vec3(105, 168, 255)/255.f;
		//vec3 colour_b = vec3(95, 232, 201)/255.f;
		vec3 colour_a = vec3(117, 248, 255)/255.f;
		vec3 colour_b = vec3(97, 126, 128)/255.f;
		colour = mix(colour_b, colour_a, t);
	}	
	FragColour = vec4(colour, 1);	
}