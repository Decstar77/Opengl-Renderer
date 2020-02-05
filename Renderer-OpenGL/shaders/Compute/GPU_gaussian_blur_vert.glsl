#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 t_coord;

void main()
{
    t_coord = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}