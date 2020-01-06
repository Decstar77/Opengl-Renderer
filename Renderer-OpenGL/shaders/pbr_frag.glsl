#version 420 core
out vec4 FragColor;

in VS_OUT {
    vec3 WorldPos;
	vec3 Normal;
    vec2 TexCoords;
	mat3 TBN;
} fs_in;

// material parameters
uniform vec3 diffuse_colour;
uniform vec3 all_colour;
uniform bool use_normal_map;

uniform vec3 camPos;

uniform sampler2D albedo_map;
uniform sampler2D all_map;
uniform sampler2D normal_map;
uniform samplerCube irradiance_map;

#define LightCount 4

layout (std140) uniform Lighting
{
    vec3 lightPositions[LightCount];
	vec3 lightColors[LightCount];
};

const float PI = 3.14159265359;
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / max(denom, 0.0000001f); // prevent divide by zero for roughness=0.0 and NdotH=1.0
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 GetNormal()
{

	//Yea this is slow but who cares right now
	if (use_normal_map)
	{
	vec3 tangentNormal = texture(normal_map, fs_in.TexCoords).xyz * 2.0 - 1.0;
	return normalize(fs_in.TBN * tangentNormal);
	}
	else
	{
		return fs_in.Normal;
	}

}
// ----------------------------------------------------------------------------
void main()
{		
    vec3 N = GetNormal();
    vec3 V = normalize(camPos - fs_in.WorldPos);



	vec3 albedo = texture(albedo_map, fs_in.TexCoords).rgb * diffuse_colour;
	//float metallic =	texture(metallic_map, TexCoords).r;

	vec3 alll = texture(all_map, fs_in.TexCoords).rgb * all_colour;

	float ao = alll.r;
	float roughness = alll.g;
	float metallic = alll.b;

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < LightCount; ++i) 
    {
        // calculate per-light radiance
        vec3 L = normalize(lightPositions[i] - fs_in.WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(lightPositions[i] - fs_in.WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightColors[i] * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
           
        vec3 nominator    = NDF * G * F; 
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
        vec3 specular = nominator / max(denominator, 0.0001); // prevent divide by zero for NdotV=0.0 or NdotL=0.0
        
        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;	  

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }   
    
    // ambient lighting (note that the next IBL tutorial will replace 
    // this ambient lighting with environment lighting).
    vec3 kS = fresnelSchlick(max(dot(N, V), 0.0), F0);
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;	  
    vec3 irradiance = texture(irradiance_map, N).rgb;
    vec3 diffuse      = irradiance * albedo;
    vec3 ambient = (kD * diffuse) * ao;    

	
	//vec3 ambient = vec3(0.03) * albedo * ao;

    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, 1.0);
}














//out vec4 FragColor;
//
//in VS_OUT {
//    vec3 WorldPos;
//    vec2 TexCoords;
//	vec3 Normal;
//} fs_in;
//
//// material parameters
//
//uniform sampler2D albedoMap;
//uniform sampler2D all_map;
//uniform sampler2D normalMap;
//
//#define LightCount 1
//
//layout (std140, binding = 1) uniform Lighting
//{
//    vec3 camPos;
//    vec3 lightPositions[LightCount];
//	vec3 lightColors[LightCount];
//};
//
//
//const float PI = 3.14159265359;
//// ----------------------------------------------------------------------------
//vec3 getNormalFromMap()
//{
//    vec3 tangentNormal = texture(normalMap, fs_in.TexCoords).xyz * 2.0 - 1.0;
//
//    vec3 Q1  = dFdx(fs_in.WorldPos);
//    vec3 Q2  = dFdy(fs_in.WorldPos);
//    vec2 st1 = dFdx(fs_in.TexCoords);
//    vec2 st2 = dFdy(fs_in.TexCoords);
//
//    vec3 N   = normalize(fs_in.Normal);
//    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
//    vec3 B  = -normalize(cross(N, T));
//    mat3 TBN = mat3(T, B, N);
//
//    return normalize(TBN * tangentNormal);
//}
//// ----------------------------------------------------------------------------
//float DistributionGGX(vec3 N, vec3 H, float roughness)
//{
//    float a = roughness*roughness;
//    float a2 = a*a;
//    float NdotH = max(dot(N, H), 0.0);
//    float NdotH2 = NdotH*NdotH;
//
//    float nom   = a2;
//    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
//    denom = PI * denom * denom;
//
//    return nom / denom;
//}
//// ----------------------------------------------------------------------------
//float GeometrySchlickGGX(float NdotV, float roughness)
//{
//    float r = (roughness + 1.0);
//    float k = (r*r) / 8.0;
//
//    float nom   = NdotV;
//    float denom = NdotV * (1.0 - k) + k;
//
//    return nom / denom;
//}
//// ----------------------------------------------------------------------------
//float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
//{
//    float NdotV = max(dot(N, V), 0.0);
//    float NdotL = max(dot(N, L), 0.0);
//    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
//    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
//
//    return ggx1 * ggx2;
//}
//// ----------------------------------------------------------------------------
//vec3 fresnelSchlick(float cosTheta, vec3 F0)
//{
//    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
//}
//// ----------------------------------------------------------------------------
//void main()
//{		
//    vec3 albedo     = pow(texture(albedoMap, fs_in.TexCoords).rgb, vec3(2.2));
//
//
//	vec3 alll = texture(all_map, fs_in.TexCoords).rgb;
//
//	float ao = alll.r;
//	float roughness = alll.g;
//	float metallic = alll.b;
//
//    vec3 N = getNormalFromMap();
//    vec3 V = normalize(camPos - fs_in.WorldPos);
//
//    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
//    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
//    vec3 F0 = vec3(0.04); 
//    F0 = mix(F0, albedo, metallic);
//
//    // reflectance equation
//    vec3 Lo = vec3(0.0);
//    for(int i = 0; i < LightCount; ++i) 
//    {
//        // calculate per-light radiance
//        vec3 L = normalize(lightPositions[i] - fs_in.WorldPos);
//        vec3 H = normalize(V + L);
//        float distance = length(lightPositions[i] - fs_in.WorldPos);
//        float attenuation = 1.0 / (distance * distance);
//        vec3 radiance = lightColors[i] * attenuation;
//
//        // Cook-Torrance BRDF
//        float NDF = DistributionGGX(N, H, roughness);   
//        float G   = GeometrySmith(N, V, L, roughness);      
//        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
//           
//        vec3 nominator    = NDF * G * F; 
//        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
//        vec3 specular = nominator / denominator;
//        
//        // kS is equal to Fresnel
//        vec3 kS = F;
//        // for energy conservation, the diffuse and specular light can't
//        // be above 1.0 (unless the surface emits light); to preserve this
//        // relationship the diffuse component (kD) should equal 1.0 - kS.
//        vec3 kD = vec3(1.0) - kS;
//        // multiply kD by the inverse metalness such that only non-metals 
//        // have diffuse lighting, or a linear blend if partly metal (pure metals
//        // have no diffuse light).
//        kD *= 1.0 - metallic;	  
//
//        // scale light by NdotL
//        float NdotL = max(dot(N, L), 0.0);        
//
//        // add to outgoing radiance Lo
//        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
//    }   
//    
//    // ambient lighting (note that the next IBL tutorial will replace 
//    // this ambient lighting with environment lighting).
//    vec3 ambient = vec3(0.03) * albedo * ao;
//    
//    vec3 color = ambient + Lo;
//
//    // HDR tonemapping
//    color = color / (color + vec3(1.0));
//    // gamma correct
//    color = pow(color, vec3(1.0/2.2)); 
//
//    FragColor = vec4(color, 1.0);
//}
//
//


//// ----------------------------------------------------------------------------
//void main()
//{		
//    vec3 N = normalize(Normal);
//    vec3 V = normalize(camPos - WorldPos);
//
//    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
//    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
//    vec3 F0 = vec3(0.04); 
//    F0 = mix(F0, albedo, metallic);
//
//    // reflectance equation
//    vec3 Lo = vec3(0.0);
//	for(int i = 0; i < 2; i++)
//    {
//        // calculate per-light radiance
//        vec3 L = normalize(-d_dir[i]);
//        vec3 H = normalize(V + L);
//        vec3 radiance = d_colour[i];
//
//        // Cook-Torrance BRDF
//        float NDF = DistributionGGX(N, H, roughness);   
//        float G   = GeometrySmith(N, V, L, roughness);      
//        vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
//           
//        vec3 nominator    = NDF * G * F; 
//        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
//        vec3 specular = nominator / max(denominator, 0.001); // prevent divide by zero for NdotV=0.0 or NdotL=0.0
//        
//        // kS is equal to Fresnel
//        vec3 kS = F;
//        // for energy conservation, the diffuse and specular light can't
//        // be above 1.0 (unless the surface emits light); to preserve this
//        // relationship the diffuse component (kD) should equal 1.0 - kS.
//        vec3 kD = vec3(1.0) - kS;
//        // multiply kD by the inverse metalness such that only non-metals 
//        // have diffuse lighting, or a linear blend if partly metal (pure metals
//        // have no diffuse light).
//        kD *= 1.0 - metallic;	  
//
//        // scale light by NdotL
//        float NdotL = max(dot(N, L), 0.0);        
//
//        // add to outgoing radiance Lo
//        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
//    }   
//    
//    // ambient lighting (note that the next IBL tutorial will replace 
//    // this ambient lighting with environment lighting).
//    vec3 ambient = vec3(0.03) * albedo * ao;
//
//    vec3 color = ambient + Lo;
//
//    // HDR tonemapping
//    color = color / (color + vec3(1.0));
//    // gamma correct
//    color = pow(color, vec3(1.0/2.2)); 
//
//    FragColor = vec4(color, 1.0);
//}

