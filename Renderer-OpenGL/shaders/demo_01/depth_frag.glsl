#version 430 core
layout (location = 0) out vec2 depth_colour;



void main()
{
    float depth = gl_FragCoord.z;
    float depth2 = depth * depth;

    depth_colour = vec2(depth, 0.45);

}


