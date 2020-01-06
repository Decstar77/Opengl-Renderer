#version 430
#extension GL_ARB_compute_shader : enable
#extension GL_ARB_shader_storage_buffer_object : enable
#extension GL_ARB_compute_variable_group_size: enable
//layout (local_size_x = 1, local_size_y = 1) in;
layout(local_size_variable) in;
layout (rgba32f, binding = 0) uniform image2D img_output;







void main() {
  // base pixel colour for image

  // get index in global work group i.e x,y position
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	ivec2 image_size = imageSize(img_output);
	float aspect = float(image_size.x)/ float(image_size.y);

	float r = gl_WorkGroupID.x /gl_NumWorkGroups.x;
	float g = gl_WorkGroupID.y /gl_NumWorkGroups.y;

	vec4 pixel = vec4(pixel_coords.x, 0, 0.0, 1.0);
	
	float gr = float(gl_GlobalInvocationID.x)/float(gl_NumWorkGroups.x);
	pixel.x = gr;

	float max_x = 5.0;
	float max_y = 5.0;
	ivec2 dims = imageSize(img_output); // fetch image dimensions
	float x = (float(pixel_coords.x * 2 - dims.x) / dims.x) * aspect;
	float y = (float(pixel_coords.y * 2 - dims.y) / dims.y);
	vec3 ray_o = vec3(x * max_x, y * max_y, 0.0);
	vec3 ray_d = vec3(0.0, 0.0, -1.0); // ortho

	vec3 sphere_c = vec3(0.0, 0.0, -10.0);
	float sphere_r = 1.0;


	vec3 omc = ray_o - sphere_c;
	float b = dot(ray_d, omc);
	float c = dot(omc, omc) - sphere_r * sphere_r;
	float bsqmc = b * b - c;
	// hit one or both sides
	if (bsqmc >= 0.0) 
	{
		pixel = vec4(0.4, 0.4, 1.0, 1.0);
	}	
  
   //interesting stuff happens here later
  
  
  // output to a specific pixel in the image
	imageStore(img_output, pixel_coords, pixel);
}