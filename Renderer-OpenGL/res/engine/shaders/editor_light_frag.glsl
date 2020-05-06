#version 420 core
out vec4 out_colour;

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


void main()
{
    vec3 colour = vec3(1);
    out_colour = vec4(colour, 1);
}