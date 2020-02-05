#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout (std140, binding = 0) uniform Matrices
{
	mat4 projection;
	mat4 view;
};

uniform mat4 model;


//@TODO Make struct
out vec3 normal;
out vec3 t_coord;

void main()
{
	normal = aNormal;
	t_coord = vec3(aTexCoords, 0);
    gl_Position =  projection * view * model * vec4(aPos, 1.0);
}
