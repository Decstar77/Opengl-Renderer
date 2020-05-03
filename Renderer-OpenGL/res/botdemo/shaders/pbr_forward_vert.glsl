#version 420 core
layout (location = 0) in vec3 vpos;
layout (location = 1) in vec3 vnormal;
layout (location = 2) in vec2 vtext;
layout (location = 3) in vec3 vtan;
layout (location = 4) in vec3 vbitan;

out vec2 texture_coords;
out vec3 world_normal;
out mat3 tbn;

layout (std140, binding = 0) uniform WorldMatrices 
{
	mat4 projection;
	mat4 view;
	mat4 light_space_matrix;
};

uniform mat4 model;

void main()
{
	texture_coords = vtext;	
	
    vec3 b = normalize(vec3(model * vec4(vbitan, 0.0)));
	vec3 n = normalize(vec3(model * vec4(vnormal,    0.0)));
	vec3 t = normalize(vec3(model * vec4(vtan,   0.0)));
	tbn  = mat3(t, b, n);
    
    world_normal = inverse( transpose( mat3(model)  ) ) * vnormal;
    gl_Position = projection * view * model * vec4(vpos, 1.0); 
}
