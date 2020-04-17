#version 420 core
layout (location = 0) in vec3 vpos;
layout (location = 1) in vec3 vnormal;
layout (location = 2) in vec2 vtext;

out vec2 texture_coords;

void main()
{
	texture_coords = vtext;	
    gl_Position = vec4(vpos, 1.0); 
}
