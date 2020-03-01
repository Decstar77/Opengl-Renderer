#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout (std140, binding = 0) uniform CameraMatrices // World matrices
{
	mat4 projection;
	mat4 view;
	mat4 light_space_matrix;
};
uniform mat4 model;
uniform mat4 light_space_matrices[3];

out VS_OUT 
{
	vec3 world_position;
	vec3 world_normal;
	vec3 texture_coords;
	vec3 camera_world_position;
	vec4 light_space_position[3];

} vs_out;



//@TODO Make struct

void main()
{
	vs_out.texture_coords = vec3(aTexCoords, 0);
    vs_out.world_position = vec3(model * vec4(aPos, 1.0));
	vs_out.world_normal = transpose(inverse(mat3(model))) * aNormal;   	
	vs_out.camera_world_position = view[3].xyz;	

	for (int i = 0; i < 3; i++)
	{
		vs_out.light_space_position[i] =  light_space_matrices[i] * vec4(vs_out.world_position, 1);
	}
	


    gl_Position =  projection * view * model * vec4(aPos, 1.0);

}
