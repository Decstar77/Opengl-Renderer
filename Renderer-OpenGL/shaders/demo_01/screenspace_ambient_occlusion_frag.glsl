#version 420
layout (location = 0) out vec4 out_colour;

in vec2 texture_coords;

uniform vec3 samples[32];
uniform mat4 projection;

uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform sampler2D noise_texture;

int kernel_size= 32;
float radius = 1.5;
float bias = 0.025;

const vec2 noise_scale = vec2(1280.0/4.0, 720.0/4.0); 

void main()
{
    vec3 view_position = texture(g_position, texture_coords).xyz;
    vec3 view_normal = texture(g_normal, texture_coords).xyz;
    vec3 rand_vec = texture(noise_texture, texture_coords * noise_scale).xyz;

    vec3 tan = normalize(rand_vec - view_normal * dot(rand_vec, view_normal));
    vec3 bitan = cross(view_normal, tan);
    mat3 tbn = mat3(tan, bitan, view_normal);

    float occ = 0.0;
    for (int i = 0; i < kernel_size; i++)
    {
        vec3 view_sample = view_position + tbn * samples[i] * radius;

        vec4 proj_sample = projection * vec4(view_sample, 1);        
        proj_sample /= proj_sample.w;
        proj_sample = proj_sample * 0.5 + vec4(0.5);
        
        vec3 texture_sample = texture(g_position, proj_sample.xy).xyz;
        float texture_depth = texture_sample.z;

        float falloff = smoothstep(0.0, 1.0, radius / abs(view_position.z - texture_depth));

        occ += (texture_depth >= view_sample.z + bias ? 1.0 : 0.0) * falloff;
    }

    occ = 1 - (occ / kernel_size);

    out_colour = vec4(occ);
}
