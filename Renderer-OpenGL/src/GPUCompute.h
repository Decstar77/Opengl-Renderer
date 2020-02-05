#pragma once
#include "OpenGL/OpenGlRenderer.h"

namespace cm
{
		
	class GPUGaussienCompute
	{
	private:
		
	public:
		FrameBuffer pingpong_fbo[2] = {};
		Mesh *quad= nullptr;
		Shader *blur= nullptr;
		Texture *texture_to_blur = nullptr;
		void CreateGPUGassien();
		void GPUGaussienBlur();

	};

	


}

