#version 420 core
out vec4 out_colour;


uniform vec3 colour;

void main()
{
    out_colour = vec4(colour, 1);
}