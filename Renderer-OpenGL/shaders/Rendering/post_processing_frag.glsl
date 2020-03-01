#version 430 core
out vec4 FragColour;

uniform sampler2D scene_texture;
uniform sampler2D bloom_texture;
uniform float exposure;

in vec2 t_coords;
void main()
{
	vec3 hdr_colour = texture(scene_texture, t_coords).rgb;
	vec3 bloom_color = texture(bloom_texture, t_coords).rgb;
	hdr_colour += bloom_color;

    
	const float gamma = 2.2;
    vec3 result = vec3(1.0) - exp(-hdr_colour * exposure);
    // also gamma correct while we're at it       
    result = pow(result, vec3(1.0 / gamma));
	
	   
	vec3 colour = result;
    FragColour = vec4(colour, 1.0);
}
