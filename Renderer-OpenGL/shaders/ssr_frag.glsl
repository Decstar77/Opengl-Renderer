#version 420 core
layout (location = 0) out vec4 FragColor;

in vec2 texture_coords;


// @NOTE: We pack the occulusion, roughness, metallic values into 
//      : g_position, g_normal and g_albedoSpec channels respectively
uniform sampler2D view_position_map;
uniform sampler2D view_normal_map;
uniform sampler2D view_colour_map;
uniform mat4 proj;



vec2 RayMarch(vec3 ray_dir, vec3 ray_origin);

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

void main()
{    
#if 0
    float max_dist = 3;
    float step_dist = 0.1;
    float tollerance = 0.5;
    float max_steps = 25;
    float near_plane = 0.1;

    vec3 vp = texture(view_position_map, texture_coords).rgb;
    vec3 vn =  texture(view_normal_map, texture_coords).rgb;
    vec2 texel_size = textureSize(view_position_map, 0).xy;
    vec2 uv_to_pixel = texel_size;
    vec2 pixel_to_uv = 1.0 / texel_size;
    vn = normalize(vn);

    vec3 colour = vec3(0);

    float roughness = texture(view_normal_map, texture_coords).w;
    if (-vp.z <= 0.01 || roughness > 0.9)
    {            
        FragColor = vec4(0, 0, 1, 1);
        return;
    }

    vec3 rd = normalize( reflect( normalize(vp), vn ) );

    vec3 view_start = vp;
    vec3 view_end = vp + rd * max_dist;

    vec4 h0 = proj * vec4(view_start, 1);
    vec4 h1 = proj * vec4(view_end, 1);

    float k0 = 1.0 / h0.w;
    float k1 = 1.0 / h1.w;

    vec3 q0 = view_start * k0;
    vec3 q1 = view_end * k1;

    vec2 p0 = h0.xy * k0;
    vec2 p1 = h1.xy * k1;

    // @NOTE: Clippong
#if 1
    {
        float x_max = 1.2f;
        float x_min = -1.2f;     
        float y_max = 1.2f;
        float y_min = -1.2f;
        
        float a = 0;

        if (p1.y > y_max || p1.y < y_min)
        {
            a =  (p1.y - ( (p1.y > y_max) ? y_max : y_min)) / (p1.y - p0.y);
        }
        if (p1.x > x_max || p1.x < x_min)
        {
            a = max(a, (p1.x - ((p1.x > x_max) ? x_max : x_min)) / (p1.x - p0.x));
        }

        p1 = mix(p1, p0, a);
        k1 = mix(k1, k0, a); 
        q1 = mix(q1, q0, a);
    }
#endif

    p1 += distance(p0, p1) < 0.001 ? 0.01 : 0.0;

    // @NOTE: To uv space
    p0 = (p0 + vec2(1)) * vec2(0.5) ;
    p1 = (p1 + vec2(1)) * vec2(0.5) ;
    
    // @NOTE: To pix space
    p0 = p0 * uv_to_pixel;
    p1 = p1 * uv_to_pixel;


    vec2 delta_dir = normalize( p1 - p0 );
    vec2 current_pix = p0;
    vec3 current_view = view_start;

    int hit_count = 0;

    for (float i = 2; i < 30; i++)
    {
        float t = ( i / 30.0);
        current_pix = mix( p0, p1, t );
        // @TODO: Perspective
        current_view = mix ( view_start, view_end, t );
        //current_view = (view_start * view_end) / mix(view_start, view_end, t);


        

        vec2 uv = current_pix * pixel_to_uv;

        float sample_depth = texture( view_position_map, uv ).z;
        float ray_depth = current_view.z;

        float delta_depth = ray_depth - sample_depth;
        
        if (sample_depth >= -0.1)
        {
            break;
        }

        if (delta_depth < 0)
        {            
            hit_count++;
        }

        if (hit_count >= 3)
        {
            float vis = 1;
            vec3 nn = normalize ( texture(view_normal_map, uv).xyz );
            vis *= 1 - max(dot(rd, normalize( -vp ) ), 0.0);
            float dx = max(dot(vn, nn), 0.0);
            dx = clamp(log(dx + 0.1) + 1, 0.0, 1.0);
            vis *= 1 - dx;
            vis *= 1 - clamp( distance( uv, p0 * pixel_to_uv ), 0.0, 1.0 );
            vis *= (uv.x < 0 || uv.x > 1) ? 0 : 1;
            vis *= (uv.y < 0 || uv.y > 1) ? 0 : 1;
            colour = vec3(1, uv) * vis;
            break;
        }

    }



    //colour = vec3(p1 - p0, 0);
    //FragColor = vec4(p0, p1);
    FragColor = vec4(colour, 1);
#else
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
    vec3 last_colour = texture(view_colour_map, texture_coords).rgb;

    if (-view_pos.z <= 0.01 || roughness > 0.9 || metallic < 0.1)
    {            
        FragColor = vec4(0, 0, 0, 0);
        return;
    }

    vec3 f0 = vec3(0.04); 
    f0      = mix(f0, last_colour, metallic);
    vec3 fresnel = FresnelSchlick(max(dot(normalize(view_normal), normalize(view_pos)), 0.0), f0);

    vec3 jitt = mix(vec3(0.0), vec3(Hash(view_pos)), roughness) * 0.5;

    jitt.x = 0;

    vec2 uv = RayMarch( normalize( ray_dir + jitt ),  view_pos);


    float vis = 1;
    vis *= 1 - max(dot(ray_dir, -to_view_pos), 0.0);
    vec3 nn = normalize ( texture(view_normal_map, uv).xyz );
    float dx = max(dot(view_normal, nn), 0.0);
    dx = clamp(log(dx + 0.1) + 1, 0.0, 1.0);
    vis *= (uv.x < 0 || uv.x > 1) ? 0 : 1;
    vis *= (uv.y < 0 || uv.y > 1) ? 0 : 1;


    vec2 dCoords = smoothstep(0.2, 0.6, abs(vec2(0.5, 0.5) - uv));
    float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);
    float ReflectionMultiplier = clamp( pow(metallic, 3.0) *  screenEdgefactor *  - ray_dir.z, 0.0, 1.0) ;

    vec3 reflected_colour = texture(view_colour_map, uv).rgb;

    colour = vec3(reflected_colour) * vis * fresnel * ReflectionMultiplier;


    FragColor = vec4(colour, vis);
#endif




}



vec2 RayMarch(vec3 ray_dir, vec3 ray_origin)
{
    float step_length = 0.1;
    float max_dist = 100;
 
    vec3 current_point = ray_origin + ray_dir;
    vec3 prev_point = current_point;
    vec3 step_inc = step_length * ray_dir;

    vec2 found_uv = vec2(-1);

    int hit_count = 0;
    
    for (int i = 0; i < max_dist; i++)
    {
        //current_point = ray_origin + step_length *  (ray_dir * (i + 2));
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

        if (hit_count >= 3)
        {
            found_uv = uv;
            break;
        }

        prev_point = current_point;
    }
 
    return vec2(found_uv);
}