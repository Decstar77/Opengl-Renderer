#version 430 core
layout (location = 0) in vec3 vpos;
layout (location = 1) in vec3 vnormal;
layout (location = 2) in vec2 vtext;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(vpos, 1.0);
} 
