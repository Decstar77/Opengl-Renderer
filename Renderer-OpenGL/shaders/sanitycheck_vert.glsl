#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;





void main()
{
    gl_Position =   vec4(aPos, 0.5); // w = 1
}
