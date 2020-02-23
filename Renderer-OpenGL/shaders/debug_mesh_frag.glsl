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
	
    float depthValue = texture(mesh_texture, fs_in.TexCoords).r;
    FragColour = vec4(vec3(depthValue), 1.0);
	//FragColour = vec4(1.0);
}
