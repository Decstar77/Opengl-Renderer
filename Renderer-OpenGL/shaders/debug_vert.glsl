#version 430
layout(location = 0) in vec4 position; // Our vec3 have a padding byte

layout (std140, binding = 0) uniform WorldMatrices
{
	mat4 projection;
	mat4 view;
};



void main()
{
	
	gl_Position =  projection * view * vec4(position.xyz, 1.0);
}



