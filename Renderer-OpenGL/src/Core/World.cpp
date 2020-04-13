#include "World.h"

namespace cm
{

	void RenderWorld(Shader *shader, Shader *batch_shader, const World &world)
	{

		//if (batch_shader != nullptr)
		//{
		//	for (int32 i = 0; i < world.batches.size(); i++)
		//	{
		//		RenderBatch(*batch_shader, world.batches[i]);
		//	}
		//}

		if (shader != nullptr)
		{
			BindShader(*shader);
			for (int32 i = 0; i < world.objects.size(); i++)
			{
				WorldObject* a = world.objects[i];
				GLMesh mesh = a->GetMeshForRender();
				Mat4 transform = a->GetTransformMatrix();

				ShaderSetMat4(shader, "model", transform.arr);
				RenderMesh(*shader, mesh);
			}
		}
	}

	void EquirectangularToCubemap::Create()
	{
		Assert(!created);
		std::string vert_src = R"(
			#version 330 core
			layout (location = 0) in vec3 vpos;

			out vec3 local_pos;

			uniform mat4 projection;
			uniform mat4 view;

			void main()
			{
				local_pos = vpos;  
				gl_Position =  projection * view * vec4(local_pos, 1.0);
			}			
		)";

		std::string frag_src = R"(
			#version 330 core
			out vec4 FragColor;
			in vec3 local_pos;

			uniform sampler2D equirectangularMap;

			const vec2 invAtan = vec2(0.1591, 0.3183);
			vec2 SampleSphericalMap(vec3 v)
			{
				vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
				uv *= invAtan;
				uv += 0.5;
				return uv;
			}

			void main()
			{		
				vec2 uv = SampleSphericalMap(normalize(local_pos)); // make sure to normalize localPos
				vec3 color = texture(equirectangularMap, uv).rgb;
    
				FragColor = vec4(color, 1.0);
			}		
		)";

		eqi_shader = CreateShader(vert_src, frag_src);

		// @NOTE: The frame buffer will be invalid until we convert the cube map
		CreateFrameBuffer(&eqi_frame);

		created = true;
	}

	void EquirectangularToCubemap::Convert(const Texture &src, CubeMap *dst)
	{
		Assert(created);
		
		BindShader(eqi_shader);

		ShaderSetMat4(&eqi_shader, "projection", projection.arr);
		ShaderBindTexture(eqi_shader, src, 0, "equirectangularMap");

		RenderCommands::ChangeViewPort(0, 0, dst->config.width, dst->config.height);
		
		for (uint32 i = 0; i < 6; ++i)
		{
			ShaderSetMat4(&eqi_shader, "view", views[i].arr);
						
			// @NOTE: A sneaky trick making a texture2d a cube map
			eqi_frame.colour0_texture_attachment.object = dst->object;
			eqi_frame.colour0_texture_attachment.config.type = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;

			// @NOTE: Changing the state of the frame buffer means we have to rebind it 
			FrameBufferBindColourAttachtments(&eqi_frame);
			BindFrameBuffer(eqi_frame);

			RenderCommands::ClearColourBuffer();
			RenderCommands::ClearDepthBuffer();

			GLMesh cube = StandardMeshes::cube;
			RenderMesh(eqi_shader, cube);
		}

		RenderCommands::ChangeViewPort(0, 0, 1280, 720);

		// @NOTE: Make sure we dont try to free somthing that was part of the cube map
		eqi_frame.colour0_texture_attachment.object = 0;

		UnbindFrameBuffer();
	}

	void EquirectangularToCubemap::Free()
	{
		Assert(created);		
		FreeShader(&eqi_shader);
		FreeFrameBuffer(&eqi_frame, true);
	}

}
