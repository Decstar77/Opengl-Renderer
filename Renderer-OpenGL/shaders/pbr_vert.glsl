#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;

layout (std140, binding = 0) uniform WorldMatrices
{
	mat4 projection;
	mat4 view;
};

uniform mat4 model;

void main()
{
    TexCoords = aTexCoords;
    WorldPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(model) * aNormal;   
	//Normal = transpose(inverse(mat3(model)))* aNormal;   

    gl_Position =  projection * view * vec4(WorldPos, 1.0);
}
