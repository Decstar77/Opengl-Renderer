#version 430
out vec4 FragColour;
in vec2 t_coords;

uniform sampler2D screen_texture;

void main()
{
    const float gamma = 2.2;
    vec3 hdrColor = texture(screen_texture, t_coords).rgb;      
    vec3 result = vec3(1.0) - exp(-hdrColor * 0.1);
    // also gamma correct while we're at it       
    result = pow(result, vec3(1.0 / gamma));
    FragColour = vec4(result, 1.0);
}
