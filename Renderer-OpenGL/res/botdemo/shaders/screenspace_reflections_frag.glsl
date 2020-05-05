#version 420 core
layout (location = 0) out vec4 FragColor;

in vec2 texture_coords;


// @NOTE: We pack the occulusion, roughness, metallic values into 
//      : g_position, g_normal and g_albedoSpec channels respectively
uniform sampler2D view_position_map;
uniform sampler2D view_normal_map;
uniform sampler2D view_colour_map;
uniform sampler2D last_frame_map;

uniform float ray_step_length;
uniform float ray_max_distance;
uniform float ray_hit_tollerance;

uniform mat4 proj;

vec3 FresnelSchlick(float cosTheta, vec3 F0);
vec2 RayMarch(vec3 ray_dir, vec3 ray_origin);
vec3 Hash(vec3 a);

void main()
{
    // @NOTE: Get all the things !!
    vec3 view_pos = texture(view_position_map, texture_coords).xyz;
    vec3 view_normal = normalize( texture(view_normal_map, texture_coords).xyz );
    vec2 texel_size = textureSize(view_position_map, 0).xy;
    vec2 uv_to_pixel = texel_size;
    vec2 pixel_to_uv = 1 / texel_size;

    vec3 to_view_pos = normalize(view_pos - vec3(0,0,0));
    vec3 ray_dir = normalize( reflect( to_view_pos, view_normal ) );
    vec3 to_camera = normalize( vec3(0,0,0) - view_pos );
    vec3 colour = vec3(0);

    float roughness = texture(view_normal_map, texture_coords).w;
    float metallic = texture(view_colour_map, texture_coords).w;
    vec3 last_colour = texture(last_frame_map, texture_coords).rgb;
    

    // @NOTE: Culling
    if (-view_pos.z <= 0.01 || roughness > 0.9 || metallic < 0.1)
    {            
        FragColor = vec4(0, 0, 0, 0);
        return;
    }

    // @NOTE: Fresnel
    vec3 f0 = vec3(0.04); 
    f0      = mix(f0, last_colour, metallic);
    vec3 fresnel = FresnelSchlick(max(dot(normalize(view_normal), normalize(view_pos)), 0.0), f0);

    // @NOTE: Jitter the ray 
    vec3 jitt = mix(vec3(0.0), vec3(Hash(view_pos)), roughness) * 0.5;
    jitt.x = 0;

    // @NOTE: Ray march !
    vec2 uv = RayMarch( normalize( ray_dir + jitt ),  view_pos);
    
    // @NOTE: Visablity
    float vis = 1;
    
    // @NOTE: Check the ray in coming towards us
    vis *= 1 - max(dot(ray_dir, -to_view_pos), 0.0);
    
    // @NOTE: Check The ray is hidding behind something
    vec3 nn = normalize ( texture(view_normal_map, uv).xyz );
    float dx = max(dot(view_normal, nn), 0.0);
    dx = clamp(log(dx + 0.1) + 1, 0.0, 1.0);
    
    // @NOTE: Check the ray went the screen
    vis *= (uv.x < 0 || uv.x > 1) ? 0 : 1;
    vis *= (uv.y < 0 || uv.y > 1) ? 0 : 1;

    // @NOTE: Smoothing
    vec2 dc = smoothstep(0.2, 0.6, abs(vec2(0.5, 0.5) - uv));
    float screen_edge = clamp(1.0 - (dc.x + dc.y), 0.0, 1.0);
    float ref = clamp( pow(metallic, 3.0) *  screen_edge *  - ray_dir.z, 0.0, 1.0) ;


    // @NOTE: Confuse
    vec3 reflected_colour = texture(view_colour_map, uv).rgb;
    colour = reflected_colour * vis * fresnel * ref;
    //FragColor = vec4(colour, 1);
    FragColor = vec4(uv, vis * ref * fresnel.x, 1);
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 Hash(vec3 a)
{
    a = fract(a * vec3(0.8, 0.8, 0.8));
    a += dot(a, a.yxz + 19.19);
    return fract((a.xxy + a.yxx)*a.zyx);
}

vec2 RayMarch(vec3 ray_dir, vec3 ray_origin)
{
    float step_length = ray_step_length;
    float max_dist = ray_max_distance;
 
    vec3 current_point = ray_origin + ray_dir;
    vec3 prev_point = current_point;
    vec3 step_inc = step_length * ray_dir;

    vec2 found_uv = vec2(-1);

    int hit_count = 0;
    
    for (int i = 0; i < max_dist; i++)
    {
        current_point = prev_point + step_inc;

        vec4 curr_proj = proj * vec4(current_point, 1);

        curr_proj.xyz /= curr_proj.w;

        vec2 uv = curr_proj.xy * 0.5 + vec2(0.5);

        vec3 sample_point = texture(view_position_map, uv).xyz;

        float sample_depth = sample_point.z;
        float current_depth = current_point.z;        

        float delta_depth = current_depth - sample_depth;

        if (ray_dir.z - delta_depth < 1)
        {        
            if (delta_depth < 0)
            {            
                hit_count++;
            }
        }

        if (hit_count >= int(ray_hit_tollerance))
        {
            found_uv = uv;
            break;
        }

        prev_point = current_point;
    }
 
    return vec2(found_uv);
}