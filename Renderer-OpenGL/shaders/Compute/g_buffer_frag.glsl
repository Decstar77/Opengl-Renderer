#version 420
layout (location = 0) out vec3 g_position;
layout (location = 1) out vec3 g_normal;
layout (location = 2) out vec4 g_albedoSpec;

in VS_OUT 
{
	vec3 world_position;
	vec3 world_normal;
	vec3 texture_coords;
	vec4 light_space_position;
} vs_in;


void main()
{    
    // Store the fragment position vector in the first gbuffer texture
    g_position = vs_in.world_position;

    // Store the per-fragment normals into the gbuffer
    g_normal = normalize(vs_in.world_normal);
    
	// Store diffuse per-fragment color
	// @HACK: Hard coded 
    g_albedoSpec.rgb = vec3(0.23, 0.48, 0.34);
    
	// Store specular intensity in gAlbedoSpec's alpha component
    g_albedoSpec.a = 0.2;
}
