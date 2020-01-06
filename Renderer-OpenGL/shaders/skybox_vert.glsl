#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VS_OUT {
    vec3 TexCoords;
	vec3 WorldPos;
} vs_out;


uniform mat4 projection;
uniform mat4 view;
uniform bool remove_translation;

void main()
{
	vs_out.WorldPos = aPos;
    vs_out.TexCoords = aPos;
	mat4 vv = remove_translation ?  mat4(mat3(view)) : view;
    gl_Position =  projection * vv * vec4(aPos, 1.0);
}


