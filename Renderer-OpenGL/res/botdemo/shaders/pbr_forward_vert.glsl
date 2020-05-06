#version 420 core
layout (location = 0) in vec3 vpos;
layout (location = 1) in vec3 vnormal;
layout (location = 2) in vec2 vtext;
layout (location = 3) in vec3 vtan;
layout (location = 4) in vec3 vbitan;


layout (std140, binding = 0) uniform WorldState 
{
	mat4 projection;
	mat4 view;
	mat4 light_space_matrix;

	// @NOTE: We pack the lighting as follows
    //      : x = PointLightCount, y = DirectionalLightCount, z = SpotLightCount
	vec4 size_data; 

    // @NOTE: The camera in world space
	vec4 camera_world_position;
	
    // @NOTE: The directional light direction in world space
	vec4 dir_light_direction;

    // @NOTE: The point light positions and colours in world space    
	vec4 point_light_position[4];
	vec4 point_light_colour[4];
		
    // @NOTE: The spot light positions, directions and colours in world space
	vec4 spot_light_position[4];
	vec4 spot_light_diections[4];
	vec4 spot_light_colours[4];
};

uniform mat4 model;

out VS_OUT 
{
	vec2 texture_coords;
	vec3 world_normal;
	mat3 tbn;
} vs_out;

void main()
{
	vs_out.texture_coords = vtext;	
	
    vec3 b = normalize(vec3(model * vec4(vbitan, 0.0)));
	vec3 n = normalize(vec3(model * vec4(vnormal,    0.0)));
	vec3 t = normalize(vec3(model * vec4(vtan,   0.0)));
	vs_out.tbn  = mat3(t, b, n);
    
    vs_out.world_normal = inverse( transpose( mat3(model)  ) ) * vnormal;
    gl_Position = projection * view * model * vec4(vpos, 1.0); 
}
