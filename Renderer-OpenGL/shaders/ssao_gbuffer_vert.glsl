#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout (std140, binding = 0) uniform  WorldMatrices
{
	mat4 projection;
	mat4 view;
	mat4 light_space_matrix;
};
uniform mat4 model;

out VS_OUT 
{
	vec3 view_position;
	vec3 view_normal;
	vec2 texture_coords;	
} vs_out;




void main()
{
    vec4 viewPos = view * model * vec4(aPos, 1.0);
    vs_out.view_position = viewPos.xyz; 
    vs_out.texture_coords= aTexCoords;
    
    mat3 normalMatrix = transpose(inverse(mat3(view * model)));
    vs_out.view_normal= normalMatrix * aNormal;
    
    gl_Position = projection * viewPos;
}
