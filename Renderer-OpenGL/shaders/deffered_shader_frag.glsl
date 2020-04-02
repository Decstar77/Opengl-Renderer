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
struct DeferredData
{
	vec3 world_position;
	vec3 world_normal;
	vec4 diffuse;
};
//---------------------------
struct Material 
{
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
//---------------------------
struct DirectionalLight 
{
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
//---------------------------
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
//---------------------------
struct SpotLight 
{
    vec3 position;
    vec3 direction;
    float cut_off;
    float outer_cut_off;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};
//---------------------------
in vec2 texture_coords;
//---------------------------
//uniform float cascade_ends[3];
uniform sampler2D position_map;
uniform sampler2D normal_map;
uniform sampler2D colour_map;
uniform sampler2D ssao_map;

Material material;
DeferredData deferred_data;
//---------------------------
float ShadowCalculation(vec4 light_space_pos, vec3 normal, vec3 light_dir)
{
	return 0;
}
//---------------------------
vec3 PhongPointLight(PointLight light, vec3 normal, vec3 view_dir)
{
    vec3 lightDir = normalize(light.position - deferred_data.world_position);
    
	// diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
   
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(view_dir, reflectDir), 0.0), material.shininess);
    
	// attenuation
    float distance = length(light.position - deferred_data.world_position);
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
//---------------------------
vec3 PhongSpotLight(SpotLight light, vec3 normal, vec3 view_dir)
{
    vec3 lightDir = normalize(light.position - deferred_data.world_position);
    
	// diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
	// specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(view_dir, reflectDir), 0.0), material.shininess);
    
	// attenuation
    float distance = length(light.position - deferred_data.world_position);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
	// spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cut_off - light.outer_cut_off;
    float intensity = clamp((theta - light.outer_cut_off) / epsilon, 0.0, 1.0);
    
	// combine results
    vec3 ambient = light.ambient * material.diffuse;
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular, TexCoords;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}
//---------------------------
vec3 PhongDirectionalLight(DirectionalLight light, vec3 normal, vec3 view_dir)
{
    vec3 light_dir = normalize(-light.direction); //Frag pos to the light

    // Diffuse shading
    float diff = max(dot(normal, light_dir ), 0.0);

    // Specular shading
    vec3 reflectDir = reflect(-light_dir , normal);
    float spec = pow(max(dot(view_dir, reflectDir), 0.0), material.shininess);

    // Combine results
    vec3 ambient = light.ambient * material.diffuse * texture(ssao_map, texture_coords).r;
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
	
	// Determine shadow
	float shadow = ShadowCalculation(vec4(1), normal, light_dir); // @HACK: Shaodow
	vec3 ds = (diffuse + specular) * (1 - shadow);


    return (ambient + ds);
}
//---------------------------
vec3 ColourPixel()
{	
	vec3 final_colour = vec3(0);

	int point_light_count =int (size_data.x);
	int directional_light_count = int (size_data.y);
	int spot_light_count = int(size_data.z);

	vec3 normal = normalize(deferred_data.world_normal);	
	vec3 view_dir = normalize(camera_world_position.xyz - deferred_data.world_position);
	


	for (int i = 0; i < point_light_count; i++)
	{
		// @HACK: Hardcoded
		PointLight light;
		light.position = vec3(-3, 2, 0);    
		light.constant = 1.0f;
		light.linear = 0.09;
		light.quadratic = 0.032;	
		light.ambient = vec3(0.05f);
		light.diffuse = vec3(0.8f);
		light.specular = vec3(1);

		final_colour += PhongPointLight(light, normal, view_dir);
	}

	for (int i = 0; i < directional_light_count; i++)
	{
		// @HACK: Hardcoded
		DirectionalLight light;
		light.direction = normalize( dir_light_direction.xyz );	
		light.ambient	= vec3(0.05f, 0.05f, 0.05f);
		light.diffuse	= vec3(0.4f, 0.4f, 0.4f);
		light.specular	= vec3(0.5f, 0.5f, 0.5f);

		final_colour += PhongDirectionalLight(light, normal, view_dir);
	}	
	
	for (int i = 0; i < spot_light_count; i++)
	{
		// @HACK: Hardcoded
		SpotLight light;
		light.position = vec3(0, 5, 0);
		light.direction = vec3 (0, -1 ,0);
		light.cut_off = cos(radians(12.5f));
		light.outer_cut_off = cos(radians(15.0f));
		light.constant  = 1.0f;
		light.linear	= 0.09;
		light.quadratic = 0.032;  
		light.ambient	= vec3(0.05f, 0.05f, 0.05f);
		light.diffuse	= vec3(0.4f, 0.4f, 0.4f);
		light.specular	= vec3(0.5f, 0.5f, 0.5f);
		
		final_colour += PhongSpotLight(light, normal, view_dir);
	}

	return final_colour;
}

void CreateGlobalState()
{
	deferred_data.world_position = texture(position_map, texture_coords).rgb;
	deferred_data.world_normal = texture(normal_map, texture_coords).rgb;
	deferred_data.diffuse = texture(colour_map, texture_coords);

	// @HACK: Hardcoded
	material.shininess = 32.f;
	material.diffuse = deferred_data.diffuse.rgb;
	material.specular = vec3(deferred_data.diffuse.a);
}

void main()
{		
	CreateGlobalState();
	vec3 colour = ColourPixel();
	FragColour = vec4(colour, 1);
}