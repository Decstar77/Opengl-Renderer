#version 430
out vec4 FragColour;

uniform sampler2D mesh_texture;

in VS_OUT {
    vec3 WorldPos;
	vec3 Normal;
    vec2 TexCoords;
} fs_in;


void main()
{
	
    FragColour = texture(mesh_texture, fs_in.TexCoords);
}
