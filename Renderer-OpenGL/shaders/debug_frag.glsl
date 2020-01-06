#version 430
out vec4 FragColour;



void main()
{

	vec3 Colour = vec3(0.1, 0.9, 0.1);
    FragColour = vec4(Colour, 1.0);
}
