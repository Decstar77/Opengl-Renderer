#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout (std140, binding = 0) uniform Matrices
{
	mat4 projection;
	mat4 view;
};

out vec3 normal;
out vec3 vertex_colour;
void main()
{
	normal = aNormal;
	vertex_colour = vec3(aTexCoords, 1);
    gl_Position =  projection * view * vec4(aPos, 1.0);
}
