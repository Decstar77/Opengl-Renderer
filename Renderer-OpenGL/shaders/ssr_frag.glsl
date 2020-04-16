#version 420 core
layout (location = 0) out vec4 FragColor;

in vec2 texture_coords;

uniform sampler2D view_position_map;
uniform sampler2D view_normal_map;

uniform mat4 proj;

vec4 RayMarch(vec3 dir, vec3 hit, float depth);

void main()
{    
    vec3 vp = texture(view_position_map, texture_coords).rgb;
    vec3 vn =  texture(view_normal_map, texture_coords).rgb;
    vn = normalize(vn);
    
    vec3 albedo = vec3(0.23, 0.48, 0.34);

    //vec4 p = proj * vec4(vp, 1);
    //p /= p.w;    


    vec3 r = normalize( reflect(normalize(vp), vn) );

    vec3 hit = vp;
    vec3 dir = r;

    vec4 ray = RayMarch(dir, hit, 0);

    vec3 colour = vec3(ray.xyz);

    FragColor = vec4(colour, 1);
}

vec4 RayMarch(vec3 dir, vec3 hit, float depth)
{
    dir *= 0.1; // Step size

    float d = 0;
    int max_steps = 30;
    for (int i = 0; i < max_steps; i++)
    {
        hit += dir; // New position in view space

        // Now we look up this new position
        vec4 pj = proj * vec4(hit, 1);
        pj /= pj.w;        
        pj = pj * 0.5 + 0.5;

        d = texture(view_position_map, pj.xy).z;

    }


    return vec4(d);
}