#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 t_coords;

void main()
{
	vec3 a = aPos;
	vec2 t = vec2((a + vec3(1))/2);
	t_coords = vec2(aTexCoords);
    gl_Position =  vec4(aPos, 1.0);
}
