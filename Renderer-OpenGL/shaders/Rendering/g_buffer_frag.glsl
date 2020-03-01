#version 420
layout (location = 0) out vec3 g_position;
layout (location = 1) out vec3 g_normal;
layout (location = 2) out vec4 g_albedoSpec;

in VS_OUT 
{
	vec3 world_position;
	vec3 world_normal;
	vec3 texture_coords;
	vec3 camera_world_position;
	vec4 light_space_position;
} vs_in;


void main()
{    
    // store the fragment position vector in the first gbuffer texture
    g_position = vs_in.world_position;
    // also store the per-fragment normals into the gbuffer
    g_normal = normalize(vs_in.world_normal);
    // and the diffuse per-fragment color
    g_albedoSpec.rgb = vs_in.texture_coords;
    // store specular intensity in gAlbedoSpec's alpha component
    g_albedoSpec.a = 0;
}
