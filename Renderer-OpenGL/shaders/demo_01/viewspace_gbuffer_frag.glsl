#version 420
// @NOTE: We pack the occulusion, roughness, metallic values into 
//      : g_position, g_normal and g_albedoSpec channels respectively
layout (location = 0) out vec4 g_position;
layout (location = 1) out vec4 g_normal;
layout (location = 2) out vec4 g_albedoSpec;

in VS_OUT 
{
	vec3 view_position;
	vec3 view_normal;
	vec2 texture_coords;
	mat3 tbn_matrix;
} vs_in;

uniform sampler2D colour_map;
uniform sampler2D normal_map;
uniform sampler2D orme_map;


void main()
{    
    vec3 orm = texture(orme_map, vs_in.texture_coords).rgb;
    vec3 n = 2.0 * texture(normal_map, vs_in.texture_coords).rgb - 1.0;
    vec3 c = texture(colour_map, vs_in.texture_coords).rgb;

    g_position.xyz = vs_in.view_position.xyz;
    g_position.a = orm.r;
     
    //vec4 vn =  vec4(vs_in.tbn_matrix * n, 1);    
    //g_normal.rgb = normalize(vn.rgb);    

    g_normal.xyz = normalize(vs_in.view_normal);
    g_normal.a = orm.g;    

    g_albedoSpec.rgb = c;
    g_albedoSpec.a = orm.b;   
}
