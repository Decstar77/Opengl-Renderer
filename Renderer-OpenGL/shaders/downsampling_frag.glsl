#version 420
layout (location = 0) out vec4 g_colour;




in vec2 texture_coords;
in float delta;

uniform sampler2D colour_map;
float t = 4;
int first;

vec3 Prefilter (vec3 c) 
{
    float brightness = max(c.r, max(c.g, c.b));
    float contribution = max(0, brightness - t);
    contribution /= max(brightness, 0.00001);
    return c * contribution;
}

vec3 SampleBox (vec2 uv, float d) 
{
    vec2 texel_size = 1.0 / textureSize(colour_map, 0).xy;
    vec4 dir = texel_size.xyxy * vec2(-d, d).xxyy;        
    vec3 s = 
         texture(colour_map, uv + dir.xy).rgb +
         texture(colour_map, uv + dir.zy).rgb +
         texture(colour_map, uv + dir.xw).rgb + 
         texture(colour_map, uv + dir.zw).rgb;
    

    return s * 0.25;
}
void main()
{    
    //vec3 colour = texture(colour_map, texture_coords).rgb;  
    vec3 colour = vec3(0);
    if (first == 1)
    {
        colour = Prefilter(SampleBox(texture_coords, delta));
    }
    else 
    {
        colour = SampleBox(texture_coords, delta);
    }

    g_colour = vec4(colour,1);
}
