#version 430 core
out vec4 out_colour;

in vec2 texture_coords;

uniform sampler2D scene_texture;
//uniform sampler2D bloom_texture;


uniform float exposure;
uniform int tonemapping_method;

uniform float vigentte_outer_radius;
uniform float vigentte_inner_radius;
uniform float vignette_intensity;

uniform int FXAA;
uniform float FXAA_SPAN_MAX;
uniform float FXAA_DIR_MIN;
uniform float FXAA_DIR_REDUC;



// From http://filmicgames.com/archives/75
vec3 Uncharted2Tonemap(vec3 x)
{
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

vec3 ReinhardToneMap(vec3 x)
{
    return (vec3(1.0) - exp(-x));
}

//http://filmicworlds.com/blog/filmic-tonemapping-operators/
vec3 FilmicToneMap(vec3 x) 
{
  vec3 X = max(vec3(0.0), x - 0.004);
  vec3 result = (X * (6.2 * X + 0.5)) / (X * (6.2 * X + 1.7) + 0.06);
  return pow(result, vec3(2.2));
}

void main()
{
    vec3 colour = texture(scene_texture, texture_coords).rgb;
    
	//vec3 bloom_color = texture(bloom_texture, texture_coords).rgb;
	//hdr_colour += bloom_color;
    
    //************************************
    // Scene Constants
    //************************************    
    vec2 screen_size = textureSize(scene_texture, 0);
    vec2 to_textel_space = 1.0/screen_size;
    float aspect = screen_size.x/screen_size.y;

    //************************************
    // FXAA Calcs
    //************************************
    if (FXAA == 1) 
    {
        // @NOTE: Get all current luminance.
        vec3 lum = vec3(0.299, 0.587, 0.114);
        float ftl = dot( texture( scene_texture, texture_coords.xy - vec2(-1, 1) * to_textel_space ).rgb, lum);
        float ftr = dot( texture( scene_texture, texture_coords.xy - vec2(1, 1) * to_textel_space ).rgb, lum);
        float fbl = dot( texture( scene_texture, texture_coords.xy - vec2(-1, -1) * to_textel_space ).rgb, lum);
        float fbr = dot( texture( scene_texture, texture_coords.xy - vec2(1, -1) * to_textel_space ).rgb, lum);
        float fm  = dot( colour, lum );

        // @NOTE: Get smallest and high luminance.
        float lmin = min(fm, min(ftl, min(ftr, min(fbl, fbr) ) ) );
        float lmax = max(fm, max(ftl, max(ftr, max(fbl, fbr) ) ) );

        // @NOTE: Calucate a direction
        vec2 dir = vec2(0);
        dir.x = -((ftl +  ftr) - (fbl + fbr));
        dir.y = ((ftl +  fbl) - (ftr + fbr));

        // @NOTE: "Normalize" the direction vector based on current settings
        float reduction = max( (ftl +  ftr + fbl + fbr) * (0.25 * FXAA_DIR_REDUC), FXAA_DIR_MIN );
        float smol = min(abs(dir.x), abs(dir.y)) + reduction;
        vec2 ndir = dir * (1.0 /  smol);
        dir = clamp(ndir, -FXAA_SPAN_MAX, FXAA_SPAN_MAX);

        // @NOTE: Convert the direction vector into texel space
        vec2 sample_point = dir * to_textel_space;
        
        // @NOTE: Blend a close sample based off the vector
        vec3 close_sample = 0.5 * (texture(scene_texture, texture_coords.xy + (sample_point * vec2(-0.25)) ).rgb +
        texture(scene_texture, texture_coords.xy + (sample_point *  vec2(0.25)) ).rgb);

        // @NOTE: Blend a far sample based off the vector
        vec3 far_sample = (close_sample * 0.5) + (0.25 * ( texture(scene_texture, texture_coords.xy + (sample_point * vec2(0.5)) ).rgb +
        texture(scene_texture, texture_coords.xy + (sample_point * vec2(0.5)) ).rgb));

        // @NOTE: Test the far sample luminance if it's out of bounds then
        //      :  fall back to the close sample, other wise use the far sample.
        float lum_far_sample = dot(lum, far_sample);
        vec3 fxaa = (lum_far_sample < lmin || lum_far_sample > lmax) ? close_sample : far_sample;
        
        colour = fxaa;
    }    
    
    //************************************
    // Tone mapping
    //************************************
    vec3 result = vec3(0);
    if (tonemapping_method == 0)
    {
        result = ReinhardToneMap(colour * exposure);    
    }
    else if (tonemapping_method == 1)
    {
        result = Uncharted2Tonemap(colour * exposure);    
    }
    else if (tonemapping_method == 2)
    {
        result = FilmicToneMap(colour * exposure);    
    }    
    

    //************************************
    // Vignette 
    //************************************
    vec2 vignette_position = (gl_FragCoord.xy / screen_size) - vec2(0.5);    
    float len = length(vignette_position);

    const vec3 vignette_colours = vec3(0.0, 0.0, 0.0); 
    float vignette_opacity = smoothstep(vigentte_inner_radius, vigentte_outer_radius, len) * vignette_intensity; 
    result.rgb = mix(result.rgb, vignette_colours, vignette_opacity);

    //************************************
    // Gamma Correction 
    //************************************
	const float gamma = 2.2;
    result = pow(result, vec3(1.0 / gamma));

    out_colour = vec4(result, 1.0);
}
