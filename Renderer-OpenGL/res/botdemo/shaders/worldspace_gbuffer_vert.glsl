#version 420 core
layout (location = 0) in vec3 vpos;
layout (location = 1) in vec3 vnormal;
layout (location = 2) in vec2 vtext;
layout (location = 3) in vec3 vtan;
layout (location = 4) in vec3 vbitan; 

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


uniform mat4 model;
uniform int view_space;

void main()
{
	vs_out.texture_coords = vtext;

	vec4 world_space_pos = model * vec4(vpos, 1.0);
    vs_out.transformed_position = world_space_pos.xyz;
		
	vs_out.transformed_normal = transpose(inverse(mat3(model))) * vnormal;
	//vs_out.world_normal = transpose(inverse(mat3(model))) * vnormal;   	

	vec3 b = normalize(vec3(model * vec4(vbitan, 0.0)));
	vec3 n = normalize(vec3(model * vec4(vnormal,    0.0)));
	vec3 t = normalize(vec3(model * vec4(vtan,   0.0)));
	vs_out.tbn_matrix  = mat3(t, b, n);

    gl_Position =  projection * view * model * vec4(vpos, 1.0);
}
