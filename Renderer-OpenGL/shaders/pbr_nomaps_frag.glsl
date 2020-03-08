#version 420
layout (location = 0) out vec4 FragColour;
layout (location = 1) out vec4 BrightColor;


layout (std140, binding = 1) uniform LightingData
{
	vec4 size_data; // x -> PointLightSize, y -> DirectionalLightSize, z-> SpotLightSize
	
	vec4 dir_light_direction;
	vec4 dir_light_color;
	
	vec4 point_light_position[4];
	vec4 point_light_color[4];
		
	vec4 spot_light_position[4];
	vec4 spot_light_diections[4];
	vec4 spot_light_colors[4];
};

in VS_OUT 
{
	vec3 world_position;
	vec3 world_normal;
	vec3 texture_coords;
	// @NOTE: This works because all primitives get the same value thus interp doesn't do anything
	vec3 camera_world_position; 		
	vec4 light_space_position;

} vs_in;


uniform vec3 light_pos;
uniform vec3 light_colour;
uniform vec3 light_direction;
uniform float light_amount;
//uniform float cascade_ends[3];
uniform sampler2D shadow_map;

float ShadowAmount(vec4 light_space_pos, vec3 normal, vec3 light_dir)
{

    // transform to [-1,1] range
    vec3 projCoords = light_space_pos.xyz / light_space_pos.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

	if(projCoords.z > 1.0)
	{
        return 0.0;
	}
	
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

	float bias = max(0.000005 * (1.0 - dot(normal, light_dir)), 0.000005); 
    // check whether current frag pos is in shadow    
	
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadow_map, 0);
	for(int x = -1; x <= 1; ++x)
	{
	    for(int y = -1; y <= 1; ++y)
	    {
	        float pcfDepth = texture(shadow_map, projCoords.xy + vec2(x, y) * texelSize).r; 
	        shadow += currentDepth + bias > pcfDepth ? 1.0 : 0.0;        
	    }    
	}
	shadow /= 9.0;
	return shadow;

}

vec3 PhongDirectionalLight(vec3 light_dir, vec3 light_colour)
{
	light_dir = normalize(-light_dir);
	
	//Diffuse
	float diff = max(dot(light_dir, vs_in.world_normal), 0.0f);
	
	//Specular
	vec3 view_dir = normalize(vs_in.camera_world_position - vs_in.world_position);
	vec3 spec_dir = reflect(-light_dir, vs_in.world_normal);
	float spec = pow(max(dot(view_dir, spec_dir), 0.f), 32) * 0.5f;

	//Shadow
	float shadow = ShadowAmount(vs_in.light_space_position, vs_in.world_normal, light_dir);
	vec3 colour = ((1 - shadow) * (diff  + spec) + vec3(0.01)) * light_colour * vs_in.texture_coords;
	
	return colour;
}

vec3 PhongPointLight(vec3 light_pos, vec3 light_colour)
{
	//TODO: Light attenuations
	vec3 light_dir = normalize(light_pos - vs_in.world_position);
	//Diffuse
	float diff = max(dot(light_dir, vs_in.world_normal), 0.0f);
	//Specular
	vec3 view_dir = normalize(vs_in.camera_world_position - vs_in.world_position);
	vec3 spec_dir = reflect(-light_dir, vs_in.world_normal);
	float spec = pow(max(dot(view_dir, spec_dir), 0.f), 32) * 0.5f;
	
	    // attenuation
    float l_distance = length(light_pos - vs_in.world_position);
    float attenuation = 1.0 / (1.0 + 0.09f * l_distance + 0.032f * (l_distance * l_distance));    

	vec3 colour = (diff  ) * light_colour * attenuation * vs_in.texture_coords;
	return colour;
}

vec3 PhongSpotLight(vec3 light_pos, vec3 spot_direction, vec3 light_colour)
{
	vec3 light_dir = normalize(light_pos - vs_in.world_position);

	float theta = dot(light_dir, normalize(-spot_direction));
	if ( theta > light_amount)
	{
		//Diffuse
		float diff = max(dot(light_dir, vs_in.world_normal), 0.0f);
		//Specular
		vec3 view_dir = normalize(vs_in.camera_world_position - vs_in.world_position);
		vec3 spec_dir = reflect(-light_dir, vs_in.world_normal);
		float spec = pow(max(dot(view_dir, spec_dir), 0.f), 32) * 0.5f;
		
		vec3 colour = (diff  + spec) * light_colour * vs_in.texture_coords;
		return colour;
	}
	else
	{
		return vec3(0);
	}
}

vec3 PhongColour()
{	
	vec3 final_colour = vec3(0);
	for (int i = 0; i < size_data.y; i++)
	{
		final_colour += PhongDirectionalLight(dir_light_direction.xyz, dir_light_color.xyz);
	}
	return final_colour;
	//return PhongSpotLight(light_pos, light_direction, light_colour);
}

void main()
{
	//Remap if t_coords are 0 - 1 
	//vec3 remap = t_coord * 2 - vec3(1, 1, 0);

	//float raduis = 0.8f;	
	//float l = length(remap) - raduis;
	
	
	vec3 colour = PhongColour();

	FragColour = vec4(colour, 1);

	float brightness = dot(FragColour.rgb, vec3(0.2126, 0.7152, 0.0722));
	BrightColor = brightness < 10.0 ? vec4(0) : vec4(FragColour.rgb, 1);
}