#version 420 core
layout (location = 0) out vec4 FragColor;

in vec2 texture_coords;

uniform sampler2D view_position_map;
uniform sampler2D view_normal_map;
uniform sampler2D view_colour_map;
uniform mat4 proj;


vec2 RayMarch(vec3 ray_dir, vec3 ray_origin);


void main()
{    
  #if 0
    float max_dist = 15;
    float step_dist = 0.1;
    float tollerance = 0.5;
    float max_steps = 25;

    vec3 vp = texture(view_position_map, texture_coords).rgb;
    vec3 vn =  texture(view_normal_map, texture_coords).rgb;
    vec2 texel_size = textureSize(view_position_map, 0).xy;
    vec2 uv_to_pixel = texel_size;
    vec2 pixel_to_uv = 1 / texel_size;
    vn = normalize(vn);

    if (-vp.z <= 0.01)
    {            
        FragColor = vec4(0, 0, 1, 1);
        return;
    }

    // @NOTE: We get the vec to the world pos from the cam pos becuase
    //      : that is what the reflect function expects
    vec3 to_pos = vp - vec3(0,0,0); // Cam is always the center of his own world QQ
    to_pos = normalize(to_pos);
    vec3 n = vn;
    
    vec3 r = normalize(reflect(to_pos, n));
    
    // @WOT: Don't we already have this ?
    vec4 pps = proj * vec4(vp, 1);
    pps /= pps.w;

    vec3 start_view = vp;
    vec2 start_uv = pps.xy * 0.5 + vec2(0.5);
    vec2 start_pix = start_uv * uv_to_pixel;

    vec4 ppe = proj * vec4(vp + (r * max_dist) , 1);
    ppe /= ppe.w;

    vec3 end_view = vp + (r * max_dist);
    vec2 end_uv = ppe.xy * 0.5  + vec2(0.5);
    vec2 end_pix = end_uv * uv_to_pixel;
    // @NOTE: Resulting uv
    vec2 uv = vec2(0);


    //end_pix = clamp(end_pix, vec2(0), vec2(texel_size.x, texel_size.y));

    // @NOTE: Get params for the march, ie step length

    vec2 current_pix  = start_pix;
    uv = current_pix * pixel_to_uv;
    
    float deltaX    = end_pix.x - start_pix.x;
    float deltaY    = end_pix.y - start_pix.y;

    float use_x = abs(deltaX) >= abs(deltaY) ? 1.0 : 0.0;
    float delta = mix(abs(deltaY), abs(deltaX), use_x) * clamp(0.03, 0, 1);

    //delta = clamp(delta, -texel_size.x, texel_size.x);

    // @NOTE: This is the step length in pixels
    vec2 march_inc = vec2(deltaX, deltaY) / max(delta, 0.001);

    // @NOTE: The distance of the current point in view_space from the camera
    float view_dist = vp.z - 0;
    float depth_delta = tollerance;

    float search0 = 0;
    float search1 = 0;

    float hit0 = 0;
    float hit1 = 0;

    // @NOTE: Begin ray marching
    for (int i = 0; i <0; ++i) 
    {

        // @NOTE: Move along the ray
        current_pix += march_inc;   

        // @NOTE: Store the current uv at the point along the ray
        uv = current_pix * pixel_to_uv;

        // @NOTE: Now we look up a new view pos from the SCENE given the new uv
        to_pos = texture(view_position_map, uv).xyz;

        // @NOTE: Get how far along the ray we are 
        search1 = mix ((current_pix.y - end_pix.y) / deltaY , (current_pix.x - start_pix.x) / deltaX, use_x);
        search1 = clamp(search1, 0, 1);

        // @NOTE: Store the current view space pos along the ray
        view_dist = (start_view.z * end_view.z) / mix(end_view.z, start_view.z, search1);

        depth_delta = view_dist - to_pos.z;
        
        if (depth_delta > 0 && depth_delta <= tollerance)
        {
            hit0 = 1;
            break;
        }
        else
        {
            search0 = search1;
        }
    }

    //FragColor = vec4(delta);
    FragColor = vec4(start_pix, end_pix);

#endif
    #if 1
    vec3 view_pos = texture(view_position_map, texture_coords).xyz;
    vec3 view_normal = normalize( texture(view_normal_map, texture_coords).xyz );
    vec2 texel_size = textureSize(view_position_map, 0).xy;
    vec2 uv_to_pixel = texel_size;
    vec2 pixel_to_uv = 1 / texel_size;


    vec3 to_view_pos = normalize(view_pos);
    vec3 ray_dir = normalize( reflect( to_view_pos, view_normal ) );
    vec3 colour = vec3(0);

    float roughness = texture(view_normal_map, texture_coords).w;

    if (-view_pos.z <= 0.01 || roughness > 0.9)
    {            
       FragColor = vec4(0, 0, 1, 1);
        return;
    }


    vec2 uv = RayMarch(ray_dir, view_pos);

    if (uv.x > 0)
    {
        float vis = 1;
        vis *= 1 - max(dot(ray_dir, -to_view_pos), 0.0);
        vis *= (uv.x < 0 || uv.x > 1) ? 0 : 1;
        vis *= (uv.y < 0 || uv.y > 1) ? 0 : 1;
       
        colour = mix(texture(view_colour_map, texture_coords).rgb, texture(view_colour_map, uv).rgb, vis);
    }
    else{
        uv = texture_coords;
        colour = texture(view_colour_map, texture_coords).rgb;
    }

    FragColor = vec4(colour, 1);
    #endif
}

vec2 DeepMarch(vec3 current_point, vec3 prev_point)
{
    return vec2(0);
}


vec2 RayMarch(vec3 ray_dir, vec3 ray_origin)
{
    float step_length = 0.1;
    float max_dist = 100;
 
    vec3 current_point = ray_origin + ray_dir;
    vec3 prev_point = ray_origin;
    vec2 found_uv = vec2(-1);

    for (int i = 0; i < max_dist; i++)
    {
        current_point = ray_origin + step_length *  (ray_dir * (i + 1));

        vec4 curr_proj = proj * vec4(current_point, 1);

        curr_proj.xyz /= curr_proj.w;

        vec2 uv = curr_proj.xy * 0.5 + vec2(0.5);

        vec3 sample_point = texture(view_position_map, uv).xyz;


        float sample_depth = sample_point.z;
        float current_depth = current_point.z;
        

        uv = (-sample_depth <= 0.01) ? vec2(-1) : uv;

        float delta_depth = current_depth - sample_depth;



        if (ray_dir.z - delta_depth < 0.4)
        {        
            if (delta_depth <= 0)
            {
                found_uv = uv;
                break;           
            }
        }

        prev_point = current_point;
    }
 
    return vec2(found_uv);
}