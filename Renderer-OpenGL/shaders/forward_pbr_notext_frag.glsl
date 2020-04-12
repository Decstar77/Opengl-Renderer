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

uniform float material_roughness;



#define pi 3.14159
#define ue_surface_reflection_zero_ncidence 0.04
#define light_count 1
float GGX(vec3 n, vec3 h, float a)
{
	float a2 = a * a;

	float hdot = max(dot(n, h), 0.0);
	float hdot2 = hdot * hdot;

	float demon = ( hdot2 * (a2 - 1.0) + 1.0);
	demon = pi * demon * demon;

	float res = a2 / demon;

	return res;
}

float GeometrySchlick(float dott, float k)
{
	// @NOTE: This is like a facny lerp
	float nume = dott;
	float demon = dott * (1 - k) + k;
	return nume/demon;
}

float GeometrySmith(vec3 normal, vec3 view, vec3 light, float k)
{
	float ndotv = max(dot(normal, view), 0.0);
	float ndotl = max(dot(normal, light), 0.0);
	
	float gg1 = GeometrySchlick(ndotv, k);
	float gg2 = GeometrySchlick(ndotl, k);

	return gg1 * gg2;
}

vec3 FresnelSchlick(float cos_theta, vec3 f0)
{
    return f0 + (1.0 - f0) * pow(1.0 - cos_theta, 5.0);
}




void main()
{
	// @NOTE: Get the lighting properties
	vec3 light_pos =  vec3(0, 5, 2);
	vec3 light_colour = vec3(23.47, 21.31, 20.79);

	// @NOTE: Calculate the world vectors
	vec3 world_pos 	= vs_in.world_position; 	
	vec3 viewDir    = normalize(camera_world_position.xyz - world_pos); // wo
	vec3 normal 	= normalize(vs_in.world_normal);
	
	// @NOTE: Get material properties
	float metallic = 0;
	float roughness = material_roughness;
	vec3 albedo = vec3(0.23, 0.48, 0.34);

	// @NOTE: Calculate constants
	float a = (roughness * roughness);
	vec3 f0 = mix(vec3(ue_surface_reflection_zero_ncidence), albedo, metallic);	
	float k = ((a + 1)*(a + 1))/8.0;

	vec3 lo = vec3(0);
	for (int i = 0; i < light_count; i++)
	{
		// @NOTE: Calculate lighting vectors
		vec3 lightDir   = normalize(light_pos - world_pos); // wi
		vec3 halfwayDir = normalize(lightDir + viewDir);

		// @NOTE: wi * n
		float ndotwi = max(dot(normal, lightDir), 0.0);
		float ndotwo = max(dot(normal, viewDir), 0.0);

		// @NOTE: Total radiance Li		
		float dist = length(light_pos - world_pos);
		float dist2 = dist * dist;

		float attun = 1.0 / dist2;

		vec3 radiance = light_colour * attun;		
		
		// @NOTE: Cook-torrance BRDF
		// @NOTE: Calculate distribution
		float d = GGX(normal, halfwayDir, a);

		// @NOTE: Calculate geometry
		float g = GeometrySmith(normal, viewDir, lightDir, k);

		// @NOTE: Calculate fersnel
		vec3 f = FresnelSchlick(max(dot(halfwayDir, viewDir), 0.0), f0);

		// @NOTE: F cook torrance -- Specular		
		vec3 fc = ( d * g * f ) / max( 4 * ndotwo * ndotwi, 0.000001);

		// @NOTE: F lambert -- Diffuse
		vec3 fl = ( albedo / pi );

		// @NOTE: Specular ratio
		vec3 ks = f; // Whys is this fresnel ?
		
		// @NOTE: Diffuse ratio
		vec3 kd  = (vec3(1) - ks) * (1 - metallic); 

		vec3 brdf = (kd * fl + fc); 

		lo += brdf * radiance * ndotwi;
	}

	//float c = GeometrySmith(normal, viewDir, lightDir, k);
	
	vec3 colour = lo;
	FragColour = vec4(colour, 1);
}