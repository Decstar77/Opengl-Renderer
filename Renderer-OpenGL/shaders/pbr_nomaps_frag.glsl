#version 420



out vec4 FragColour;

in vec3 normal;
in vec3 vertex_colour;

void main()
{
	FragColour = vec4(vertex_colour, 1);
}