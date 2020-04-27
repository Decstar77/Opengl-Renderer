#version 420 core
out vec4 out_colour;


in vec2 texture_coords;

void main()
{
    vec3 colour = vec3(0);
    colour += (texture_coords.x < 0.5 && texture_coords.y > 0.5) ? vec3(1, 0, 0) : vec3(0);
    colour += (texture_coords.x < 0.5 && texture_coords.y < 0.5) ? vec3(0, 0, 1) : vec3(0);
    colour += (texture_coords.x > 0.5 && texture_coords.y > 0.5) ? vec3(0, 1, 0) : vec3(0);
    colour += (texture_coords.x > 0.5 && texture_coords.y < 0.5) ? vec3(1, 1, 1) : vec3(0);
    out_colour = vec4(colour, 1);
}