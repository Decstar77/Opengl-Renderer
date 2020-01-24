#version 430
#extension GL_ARB_compute_shader : enable
#extension GL_ARB_shader_storage_buffer_object : enable
#extension GL_ARB_compute_variable_group_size: enable
//layout (local_size_x = 1, local_size_y = 1) in;
layout(local_size_variable) in;


layout(std430,binding=4) buffer PosBlock {
	vec4 Pos[];
};

layout(std430,binding=5) buffer VelBlock {
	vec4 Vel[];
};

layout(std430,binding=6) buffer ColBlock {
	vec4 Col[];
};
uint gid = gl_GlobalInvocationID.x;
vec3 p = Pos[0].xyz;

void main()
{
Pos[0].x = 0;

}