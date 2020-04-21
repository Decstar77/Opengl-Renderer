#version 420 core
layout (location = 0) out vec4 FragColor;

in vec2 texture_coords;

uniform sampler2D view_position_map;
uniform sampler2D view_normal_map;

uniform mat4 proj;

uniform vec3 cam_pos;

vec4 RayMarch(vec3 dir, vec3 hit, float depth);

void main()
{    
    vec3 vp = texture(view_position_map, texture_coords).rgb;
    vec3 vn =  texture(view_normal_map, texture_coords).rgb;
    vn = normalize(vn);
    
    // @NOTE: We get the vec to the world pos from the cam pos becuase
    //      : that is what the reflect function expects
    vec3 to_pos = vp - vec3(0,0,0); // Cam is always the center of his own world QQ
    to_pos = normalize(to_pos);
    vec3 n = vn;
    
    vec3 r = normalize(reflect(to_pos, n));


    FragColor = vec4(r  , 1);
}

