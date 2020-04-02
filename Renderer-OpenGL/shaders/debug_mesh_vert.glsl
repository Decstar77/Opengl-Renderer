#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
    vec3 WorldPos;	
	vec3 Normal;
    vec2 TexCoords;
} vs_out;

layout (std140, binding = 0) uniform WorldMatrices
{
	mat4 projection;
	mat4 view;
};

uniform mat4 model;
void main()
{
    vs_out.TexCoords = aTexCoords;
    vs_out.WorldPos = vec3(model * vec4(aPos, 1.0));
	vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;   
	
    gl_Position = vec4(vs_out.WorldPos, 1.0);
}

