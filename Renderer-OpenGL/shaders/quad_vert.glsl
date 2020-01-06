#version 430
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 textCoords;


out vec2 t_coords;

void main()
{

	t_coords = textCoords;
	gl_Position = vec4(position, 1);
}

