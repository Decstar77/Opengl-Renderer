#version 330 core
layout (location = 0) out vec4 FragColour;
layout (location = 1) out vec4 BrightColor;

in vec3 WorldPos;

uniform samplerCube environmentMap;

void main()
{	
	//*********environment mapping**************//
    //vec3 envColor = textureLod(environmentMap, WorldPos, 1.2).rgb;    
    // HDR tonemap and gamma correct
    //envColor = envColor / (envColor + vec3(1.0));
    //envColor = pow(envColor, vec3(1.0/2.2)); 
    //FragColor = vec4(envColor, 1.0);    


	
	
	vec3 dir  = normalize(WorldPos);
	vec3 up  = vec3(0, 1, 0);
	float value =  dot(dir, up);

	float t = (value + 1) /2.f;
	
	//vec3 colour_a = vec3(105, 168, 255)/255.f;
	//vec3 colour_b = vec3(95, 232, 201)/255.f;
	vec3 colour_a = vec3(117, 248, 255)/255.f;
	vec3 colour_b = vec3(97, 126, 128)/255.f;
	
	//float a = 0.0f;
	//float b = 0.0f;
    //vec3 colour_a = vec3(a);
    //vec3 colour_b = vec3(b);

	vec3 colour = vec3(0);
	colour = mix(colour_b, colour_a, t);
	//colour = colour_a;

	FragColour = vec4(colour, 1);
	//Don't write to the bright buffer;
	BrightColor = vec4(0);
}