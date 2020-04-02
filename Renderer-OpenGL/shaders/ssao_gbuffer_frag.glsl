#version 420 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;

in VS_OUT 
{
	vec3 view_position;
	vec3 view_normal;
	vec2 texture_coords;	
} vs_in;

void main()
{    
    // store the fragment position vector in the first gbuffer texture
    gPosition = vs_in.view_position;
    
	// also store the per-fragment normals into the gbuffer
    gNormal = normalize(vs_in.view_normal);

    // and the diffuse per-fragment color
    gAlbedo.rgb = vec3(0.95);
}