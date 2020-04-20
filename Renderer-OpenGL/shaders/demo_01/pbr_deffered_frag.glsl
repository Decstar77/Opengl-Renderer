#version 420
layout (location = 0) out vec4 out_colour;


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


in vec2 texture_coords;

// @NOTE: We pack the occulusion, roughness, metallic values into 
//      : g_position, g_normal and g_albedoSpec channels respectively
uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform sampler2D g_colour;
uniform sampler2D g_ssao;

// @NOTE: Defines

#define pi 3.14159

//-----------------------------------------------------------------
vec3 FresnelSchlick(float cos_theta, vec3 f0);
//-----------------------------------------------------------------
float DistGGX(vec3 n, vec3 h, float a);
//-----------------------------------------------------------------
float GeometrySchlick(float dott, float k);
//-----------------------------------------------------------------
float GeometrySmith(vec3 normal, vec3 view, vec3 light, float k);
//-----------------------------------------------------------------

void main()
{		
    vec3 colour = vec3(0);
    // @NOTE: Get world details
    vec3 world_pos = texture(g_position, texture_coords).xyz;
    vec3 world_normal = normalize( texture(g_normal, texture_coords).xyz );
    vec3 view_direction = normalize(camera_world_position.xyz - world_pos);

    // @NOTE: Get material details, now artist make albedo in sRGB so we corrects it
    vec3 albedo = pow (texture(g_colour, texture_coords).rgb, vec3(2.2));
    float ao = texture(g_position, texture_coords).a;
    float roughness = texture(g_normal, texture_coords).a;
    float metallic = texture(g_colour, texture_coords).a;
	float ssao = texture(g_ssao, texture_coords).r;

    vec3 light_pos = point_light_position[0].xyz;
    vec3 light_colour = point_light_colour[0].rgb;

    // @NOTE: Get constants
    float a = (roughness * roughness);
	float k = ((a + 1)*(a + 1))/8.0;
	vec3 f0 = mix(vec3(0.04), albedo, metallic);	
    vec3 n = world_normal;
    
    vec3 lo = vec3(0);
    for (int i = 0; i < 1; i++)
    {
        // @NOTE: Incoming ray
        vec3 wi = normalize(light_pos - world_pos);
        // @NOTE: Out going ray. This is also the half way vector
        vec3 wo = normalize(wi + view_direction);
        
        // @NOTE: Dots
		float ndotwi = max(dot(n, wi), 0.0);
		float ndotwo = max(dot(n, wo), 0.0);

		// @NOTE: Total radiance Li		
		float dist = length(light_pos - world_pos);
		float dist2 = dist * dist;
		float attun = 1.0 / dist2;
		vec3 radiance = light_colour * attun;	

		// @NOTE: Cook-torrance BRDF
		// @NOTE: Calculate distribution
		float d = DistGGX(n, wo, a);

		// @NOTE: Calculate geometry
		float g = GeometrySmith(n, view_direction, wi, k);

		// @NOTE: Calculate fersnel
		vec3 f = FresnelSchlick(max(dot(wo, view_direction), 0.0), f0);

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
    vec3 ambient = vec3(0.03) * albedo * ao * ssao;
    colour = ambient + lo;




	out_colour = vec4(colour, 1);
}


// @NOTE: Fresnel functions
vec3 FresnelSchlick(float cos_theta, vec3 f0)
{
    return f0 + (1.0 - f0) * pow(1.0 - cos_theta, 5.0);
}

// @NOTE: Normal dist functions
float DistGGX(vec3 n, vec3 h, float a)
{
	float a2 = a * a;

	float hdot = max(dot(n, h), 0.0);
	float hdot2 = hdot * hdot;

	float demon = ( hdot2 * (a2 - 1.0) + 1.0);
	demon = pi * demon * demon;

	float res = a2 / demon;

	return res;
}

// @NOTE: Geometry functions
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
