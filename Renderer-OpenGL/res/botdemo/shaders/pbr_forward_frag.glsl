#version 420 core
layout (location = 0) out vec4 g_colour;

in vec2 texture_coords;
in vec3 world_normal;
in mat3 tbn;

layout (std140, binding = 1) uniform LightingData
{
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

uniform sampler2D colour_map;
uniform sampler2D normal_map;
uniform sampler2D orme_map;


void main()
{	
	vec4 colour = vec4(0);

    colour = texture(colour_map, texture_coords).rgba;
    colour.a *= 2;
    if (colour.a < 0.9)
        discard;

    vec3 light_pos = point_light_position[0].xyz;
    vec3 light_colour = point_light_colour[0].rgb;

    vec3 dir = normalize( light_pos - camera_world_position.xyz );    
    vec3 normal = 2.0 * texture(normal_map, texture_coords).rgb - 1.0;
    normal = normalize( tbn * normal );

    float diff = abs( dot( dir, normal ) ) + 0.1f;

    colour *= diff;
    //colour = vec4(normal, 1);
	
    g_colour = colour;	
}