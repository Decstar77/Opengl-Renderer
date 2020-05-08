#version 420 core
out vec4 out_colour;

void main()
{
    vec3 colour = vec3(1);
    out_colour = vec4(colour, 1);
}