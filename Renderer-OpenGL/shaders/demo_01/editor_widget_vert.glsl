#version 420 core
layout (location = 0) in vec3 vpos;
layout (location = 1) in vec3 vnormal;
layout (location = 2) in vec2 vtext;


layout (std140, binding = 0) uniform WorldMatrices 
{
	mat4 projection;
	mat4 view;
	mat4 light_space_matrix;
};

uniform mat4 model;

void main()
{
    gl_Position =  projection * view * model * vec4(vpos, 1.0);
}
