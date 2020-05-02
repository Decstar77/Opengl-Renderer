#version 420 core
layout (location = 0) out vec4 g_colour;

in vec2 texture_coords;

uniform sampler2D test_texture;

void main()
{	
	vec4 colour = vec4(0);

    colour = texture(test_texture, texture_coords).rgba;
    colour.a *= 2;

	g_colour = colour;	
}