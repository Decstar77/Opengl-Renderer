#version 420 core
layout (location = 0) out float blur_colour;

in vec2 texture_coords;
  
uniform sampler2D texture_to_blur;

void main() {
    vec2 texel_size = 1.0 / vec2(textureSize(texture_to_blur, 0));
	const int dime = 2; // @NOTE: This is the texture how much to blur

    float result = 0.0;
    for (int x = -dime; x < dime; ++x) 
    {
        for (int y = -dime; y < dime; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texel_size;
            result += texture(texture_to_blur, texture_coords + offset).r;
        }
    }
   blur_colour = result / ((dime + dime) * (dime + dime));   
} 