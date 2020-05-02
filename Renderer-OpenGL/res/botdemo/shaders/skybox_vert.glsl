#version 330 core
layout (location = 0) in vec3 vpos;

uniform mat4 projection;
uniform mat4 view;

out vec3 world_position;

void main()
{
    world_position = vpos;

	mat4 rot = mat4(mat3(view));
	vec4 clip = projection * rot * vec4(world_position, 1.0);

	gl_Position = clip.xyww;
}
