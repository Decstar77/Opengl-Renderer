#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec4 BoneIDs; 
layout (location = 4) in vec4 Weights;


layout (std140, binding = 0) uniform  WorldMatrices 
{
	mat4 projection;
	mat4 view;
	mat4 light_space_matrix;
};



out VS_OUT 
{
	vec3 world_position;
	vec3 world_normal;
	vec3 texture_coords;

	vec4 light_space_position;
} vs_out;


uniform mat4 model;
uniform mat4 gBones[50];
void main()
{
	vs_out.texture_coords = vec3(aTexCoords, 0);
    vs_out.world_position = vec3(model * vec4(aPos, 1.0));
	vs_out.world_normal = transpose(inverse(mat3(model))) * aNormal;   	

	vs_out.light_space_position =  light_space_matrix * vec4(vs_out.world_position, 1);
	
	mat4 BoneTransform = gBones[int(BoneIDs[0])] * Weights[0];
    BoneTransform     += gBones[int(BoneIDs[1])] * Weights[1];
    BoneTransform     += gBones[int(BoneIDs[2])] * Weights[2];
    BoneTransform     += gBones[int(BoneIDs[3])] * Weights[3];



    gl_Position =  projection * view * model * BoneTransform * vec4(aPos, 1.0);

}
