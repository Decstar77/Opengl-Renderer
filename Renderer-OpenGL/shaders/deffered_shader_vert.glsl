#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout (std140, binding = 0) uniform  WorldMatrices // World matrices
{
	mat4 projection;
	mat4 view;
	mat4 light_space_matrix;
};


out vec2 texture_coords;

void main()
{
	texture_coords = aTexCoords;	
    gl_Position = vec4(aPos, 1.0); 
}
