#version 420
layout (location = 0) out vec4 FragColour;
layout (location = 1) out vec4 BrightColor;


layout (std140, binding = 1) uniform LightingData
{
	vec4 size_data; // x -> PointLightSize, y -> DirectionalLightSize, z-> SpotLightSize
	
	vec4 camera_world_position;
	
	vec4 dir_light_direction;
	vec4 camera_pos;     
	
	vec4 point_light_position[4];
	vec4 point_light_color[4];
		
	vec4 spot_light_position[4];
	vec4 spot_light_diections[4];
	vec4 spot_light_colors[4];
};
//---------------------------
in VS_OUT 
{
	vec3 world_position;
	vec3 world_normal;
	vec3 texture_coords;
	vec4 light_space_position;

} vs_in;


#define pi 3.14159

struct PointLight 
{
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material 
{
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

Material material;

vec3 PhongPointLight(PointLight light, vec3 normal, vec3 view_dir)
{
    vec3 lightDir = normalize(light.position - vs_in.world_position);
    
	// diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
   
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(view_dir, reflectDir), 0.0), 32);
    
	// attenuation
    float distance = length(light.position - vs_in.world_position);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
	// combine results    
	vec3 ambient = light.ambient *			material.diffuse;
    vec3 diffuse = light.diffuse * diff *	material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}



void main()
{
	// @NOTE: Get the lighting properties
	vec3 light_pos =  vec3(0, 5, 2);
	vec3 light_colour = vec3(23.47, 21.31, 20.79) / 25.0;

	// @NOTE: Calculate the world vectors
	vec3 world_pos 	= vs_in.world_position; 	
	vec3 viewDir    = normalize(camera_world_position.xyz - world_pos); // wo
	vec3 normal 	= normalize(vs_in.world_normal);
	vec3 lightDir   = normalize(light_pos - world_pos); // wi
	// @NOTE: Get material properties
	vec3 albedo = vec3(0.23, 0.48, 0.34);
    vec3 spec = vec3(0.02);


    material.diffuse = albedo;
    material.specular = spec;

    PointLight light;
    light.position = light_pos;    
    light.constant = 1.0f;
    light.linear = 0.09;
    light.quadratic = 0.032;	
    light.ambient = vec3(0.01f);
    light.diffuse = light_colour;
    light.specular = vec3(1);
    
    vec3 lo = PhongPointLight(light, normal, viewDir);

	vec3 colour = lo;
	FragColour = vec4(colour, 1);
}