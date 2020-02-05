#version 420
layout (location = 0) out vec4 FragColour;
layout (location = 1) out vec4 BrightColor;


in vec3 normal;
in vec3 t_coord;

void main()
{
	//Remap if t_coords are 0 - 1 
	//vec3 remap = t_coord * 2 - vec3(1, 1, 0);
	vec3 remap  = vec3(5.8, 0.3, 0.4);
	//float raduis = 0.8f;	
	//float l = length(remap) - raduis;

	FragColour = vec4(remap, 1);

	float brightness = dot(FragColour.rgb, vec3(0.2126, 0.7152, 0.0722));
	BrightColor = brightness < 1.0 ? vec4(0) : vec4(FragColour.rgb, 1);
}