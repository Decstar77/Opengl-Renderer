#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 instanceMatrix;

layout (std140, binding = 0) uniform CameraMatrices
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
	vec3 camera_world_position;	
	vec4 light_space_position;

} vs_out;

void main()
{
	vs_out.texture_coords = vec3(aTexCoords, 0);
    vs_out.world_position = vec3(instanceMatrix * vec4(aPos, 1.0));
	vs_out.world_normal = transpose(inverse(mat3(instanceMatrix))) * aNormal;   
	vs_out.camera_world_position = view[3].xyz;



    gl_Position =  projection * view * instanceMatrix * vec4(aPos, 1.0);
}
