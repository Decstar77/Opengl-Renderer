#version 420 core
layout (location = 0) in vec3 vpos;
layout (location = 1) in vec3 vnormal;
layout (location = 2) in vec2 vtext;
layout (location = 3) in vec3 vtan;
layout (location = 4) in vec3 vbitan; 
layout (location = 5) in vec4 vboneindex;
layout (location = 6) in vec4 vbonewieght;

layout (std140, binding = 0) uniform WorldMatrices 
{
	mat4 projection;
	mat4 view;
	mat4 light_space_matrix;
};

out VS_OUT 
{
	vec3 transformed_position;
	vec3 transformed_normal;
	vec2 texture_coords;
	mat3 tbn_matrix;
} vs_out;

#define BONE_COUNT 60

uniform int view_space;
uniform mat4 model;
uniform mat4 gBones[BONE_COUNT];


// @TODO: View space things

void main()
{	
	mat4 bone_transform = gBones[int(vboneindex[0])] * vbonewieght[0];
    bone_transform     += gBones[int(vboneindex[1])] * vbonewieght[1];
    bone_transform     += gBones[int(vboneindex[2])] * vbonewieght[2];
    bone_transform     += gBones[int(vboneindex[3])] * vbonewieght[3];

	vec3 b = normalize(vec3(model * bone_transform * vec4(vbitan, 0.0)));
	vec3 n = normalize(vec3(model * bone_transform * vec4(vnormal, 0.0)));
	vec3 t = normalize(vec3(model * bone_transform * vec4(vtan, 0.0)));
	vs_out.tbn_matrix  = mat3(t, b, n);

	vec4 local_pos = bone_transform * vec4(vpos, 1.0);
	vec4 world_pos = model * local_pos;

	vs_out.texture_coords = vtext;
    vs_out.transformed_position = world_pos.xyz;
    vs_out.transformed_normal = transpose(inverse(mat3(model * bone_transform))) * vnormal;

	gl_Position = projection * view * world_pos;
}