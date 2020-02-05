#include "GPUCompute.h"

namespace cm
{	
	void GPUGaussienCompute::CreateGPUGassien()
	{
		Assert(texture_to_blur != nullptr);
		Assert(quad != nullptr);
		Assert(blur != nullptr);
		Assert(texture_to_blur->object != 0);

		uint32 width = texture_to_blur->config.width;
		uint32 height = texture_to_blur->config.height;

		CreateFrameBuffer(&pingpong_fbo[0]);
		CreateFrameBuffer(&pingpong_fbo[1]);

		pingpong_fbo[0].colour0_texture_attachment.config = texture_to_blur->config;
		pingpong_fbo[1].colour0_texture_attachment.config = texture_to_blur->config;

		CreateTexture(&pingpong_fbo[0].colour0_texture_attachment, nullptr);
		CreateTexture(&pingpong_fbo[1].colour0_texture_attachment, nullptr);

		FrameBufferAddColourAttachtments(&pingpong_fbo[0]);
		FrameBufferAddColourAttachtments(&pingpong_fbo[1]);

		Assert(CheckFrameBuffer(pingpong_fbo[0]));
		Assert(CheckFrameBuffer(pingpong_fbo[1]));
	}

	void GPUGaussienCompute::GPUGaussienBlur()
	{
		bool horizontal = true;
		bool first_iteration = true;
		uint32 amount = 10;
		BindShader(*blur);
		for (unsigned int i = 0; i < amount; i++)
		{
			BindFrameBuffer(pingpong_fbo[horizontal]);
			ShaderSetInt32(*blur, "horizontal", static_cast<int32>(horizontal));
			ShaderBindTexture(*blur, first_iteration ? *texture_to_blur : pingpong_fbo[!horizontal].colour0_texture_attachment, 0, "image");
			RenderMesh(*blur, *quad);
			horizontal = !horizontal;
			if (first_iteration)
			{
				first_iteration = false;
			}
		}		
		UnbindFrameBuffer();

		Texture blured_texture =  pingpong_fbo[!horizontal].colour0_texture_attachment;

		CopyTexture(&blured_texture, texture_to_blur); 

		//return  pingpong_fbo[0].colour0_texture_attachment;
	}

}
