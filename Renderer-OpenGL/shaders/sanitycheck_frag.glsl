#version 420
layout (location = 0) out vec4 colour;



void main()
{    
	colour = vec4(vec3(gl_FragCoord.x/1280), 1);
}
