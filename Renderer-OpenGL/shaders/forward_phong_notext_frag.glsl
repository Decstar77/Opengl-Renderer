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
	mat3 tbn_matrix;
	vec4 light_space_position;

} vs_in;
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

uniform vec3 diffuse_colour;
uniform vec3 specular_colour;
uniform bool use_normal_map;

//uniform float cascade_ends[3];
uniform sampler2D shadow_map;
uniform sampler2D colour_map;
uniform sampler2D normal_map;
uniform sampler2D oc_r_m_map;

Material material;
//---------------------------
float PCF(vec4 light_space_pos, vec3 normal, vec3 light_dir)
{
    // transform to [-1,1] range, NDC
    vec3 projCoords = light_space_pos.xyz / light_space_pos.w;
    // transform to [0,1] range, IE The screen space coords. 
    projCoords = projCoords * 0.5 + 0.5;

	if(projCoords.z > 1.0)
	{
        return 0.0;
	}
	
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

	float smol = .1f;
	float bias = max(smol * 10.f * (1.0 - dot(normal, light_dir)), smol); 
    // check whether current frag pos is in shadow    
	
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadow_map, 0);
	for(int x = -1; x <= 1; ++x)
	{
	    for(int y = -1; y <= 1; ++y)
	    {
	        float pcfDepth = texture(shadow_map, projCoords.xy + vec2(x, y) * texelSize).r; 
	        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
	    }    
	}
	shadow /= 9.0;
	return shadow;
}

float linstep(float low, float high, float v)
{
	return clamp((v-low)/(high-low), 0.0, 1.0);
}


float Variance(vec4 light_space_pos, vec3 normal, vec3 light_dir)
{
	// transform to [-1,1] range, NDC
    vec3 projCoords = light_space_pos.xyz / light_space_pos.w;
    // transform to [0,1] range, IE The screen space coords. 
    projCoords = projCoords * 0.5 + 0.5;
	if(projCoords.z > 1.0)
	{
        return 0;
	}
	vec2 moments = texture(shadow_map, projCoords.xy).xy;

	float depth = projCoords.z;

	float p = step(depth, moments.x);
	float v = max (moments.y - moments.x * moments.x, 0.00000002);
	float d = depth - moments.x;
	
	float pmax = linstep(0.2, 1.0, v / (v + d * d));
	
	return 1 - min(max(p, pmax), 1);
	return max(p,  (depth <= moments.x) ? 1.f : 0.f);

}

float ShadowCalculation(vec4 light_space_pos, vec3 normal, vec3 light_dir)
{

	//return PCF(light_space_pos, normal, light_dir);
	return Variance(light_space_pos, normal, light_dir);
}
//---------------------------
vec3 PhongPointLight(PointLight light, vec3 normal, vec3 view_dir)
{
    vec3 lightDir = normalize(light.position - vs_in.world_position);
    
	// diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
   
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(view_dir, reflectDir), 0.0), material.shininess);
    
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
//---------------------------
vec3 PhongSpotLight(SpotLight light, vec3 normal, vec3 view_dir)
{
    vec3 lightDir = normalize(light.position - vs_in.world_position);
    
	// diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
	// specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(view_dir, reflectDir), 0.0), material.shininess);
    
	// attenuation
    float distance = length(light.position - vs_in.world_position);
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
    vec3 ambient = light.ambient * material.diffuse;
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
	
	// Determine shadow
	float shadow = ShadowCalculation(vs_in.light_space_position, normal, light_dir);
	vec3 ds = (diffuse + specular) * (1 - 0);


    return (ambient + ds);
}
//---------------------------
vec3 ColourPixel()
{	
	vec3 final_colour = vec3(0);

	int point_light_count =int (size_data.x);
	int directional_light_count = int (size_data.y);
	int spot_light_count = int(size_data.z);


	
	// @HACK: Hardcoded
	material.shininess = 32.f;
	material.diffuse = diffuse_colour * texture(colour_map, vs_in.texture_coords.xy).xyz;
	material.specular = specular_colour * texture(oc_r_m_map, vs_in.texture_coords.xy).z;
	vec3 normal = vec3(0);
	if (use_normal_map)
	{ 
	    vec3 n = texture(normal_map, vs_in.texture_coords.xy).rgb;
		n = n * 2.0f - 1.0f;
		normal = normalize(vs_in.tbn_matrix * n);	
	}		
	else
	{
		normal = normalize(vs_in.world_normal);
	}
	
	vec3 view_dir = normalize(camera_world_position.xyz - vs_in.world_position);

	for (int i = 0; i < point_light_count; i++)
	{
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
		DirectionalLight light;
		light.direction = normalize( dir_light_direction.xyz );	
		light.ambient	= vec3(0.05f, 0.05f, 0.05f);
		light.diffuse	= vec3(0.4f, 0.4f, 0.4f);
		light.specular	= vec3(0.5f, 0.5f, 0.5f);

		final_colour += PhongDirectionalLight(light, normal, view_dir);
	}	
	
	for (int i = 0; i < spot_light_count; i++)
	{
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

void main()
{		
	vec3 colour = ColourPixel();
	FragColour = vec4(colour, 1);
	float brightness = dot(FragColour.rgb, vec3(0.2126, 0.7152, 0.0722));
	BrightColor = brightness < 10.0 ? vec4(0) : vec4(FragColour.rgb, 1);
}