#version 420 core
out vec4 FragColor;

in vec2 t_coord;

uniform sampler2D image;

uniform bool horizontal;
uniform float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

void main()
{             
	 vec2 remap = t_coord;
     vec2 tex_offset = 1.0 / textureSize(image, 0); // gets size of single texel
     vec3 result = texture(image, remap).rgb * weight[0];
     if(horizontal)
     {
         for(int i = 1; i < 5; ++i)
         {
            result += texture(image, remap + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(image, remap - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
         }
     }
     else
     {
         for(int i = 1; i < 5; ++i)
         {
             result += texture(image, remap + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
             result += texture(image, remap - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
         }
     }
	
	 vec3 colour = result;
	 //vec3 colour = vec3(remap, 0);
     FragColor = vec4(colour, 1.0);
}