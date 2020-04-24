#pragma once
#include "glew.h"
#include "Core.h"
#include "Math/CosmicMath.h" //YEET OUT Colour can be an uint32



// @NOTE: Depricated will remove when I rewrite rendere
// @NOTE: Depricated will remove when I rewrite rendere
// @NOTE: Depricated will remove when I rewrite rendere
// @NOTE: Depricated will remove when I rewrite rendere
// @NOTE: Depricated will remove when I rewrite rendere
// @NOTE: Depricated will remove when I rewrite rendere
// @NOTE: Depricated will remove when I rewrite rendere
// @NOTE: Depricated will remove when I rewrite rendere
// @NOTE: Depricated will remove when I rewrite rendere
// @NOTE: Depricated will remove when I rewrite rendere
// @NOTE: Depricated will remove when I rewrite rendere
// @NOTE: Depricated will remove when I rewrite rendere
// @NOTE: Depricated will remove when I rewrite rendere
// @NOTE: Depricated will remove when I rewrite rendere
namespace cm
{
	class RenderCommands // @NOTE: Depricated will remove when I rewrite rendere
	{
	public:
		static inline void EnableMulitSampling()
		{
			glEnable(GL_MULTISAMPLE);
		}

		static inline void ClearLogger(Vec4 colour)
		{
			glClearColor(colour.x, colour.y, colour.z, colour.w);
		}

		static inline void EnableDepthBuffer()
		{
			glEnable(GL_DEPTH_TEST);
		}

		static inline void EnableCubeMapSeamless()
		{
			glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		}

		static inline void DepthBufferFunction(uint32 func)
		{
			glDepthFunc(func);
		}

		static inline void Enable(uint32 cmd)
		{
			glEnable(cmd);
		}

		static inline void EnableStencilBuffer()
		{
			glEnable(GL_STENCIL_TEST);
		}

		static inline void DisableDepthBuffer()
		{
			glDisable(GL_DEPTH_TEST);
		}

		static inline void EnableDepthBufferWriting()
		{
			glDepthMask(GL_FALSE);
		}

		static inline void DisableDepthBufferWriting()
		{
			glDepthMask(GL_FALSE);
		}

		static inline void DisbleStencilBuffer()
		{
			glDisable(GL_STENCIL_TEST);
		}

		static inline void ClearAllBuffers()
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		}

		static inline void ClearColourBuffer()
		{
			glClear(GL_COLOR_BUFFER_BIT);
		}

		static inline void ClearDepthBuffer()
		{
			glClear(GL_DEPTH_BUFFER_BIT);
		}

		static inline void EnableFaceCulling()
		{
			glEnable(GL_CULL_FACE);
			glFrontFace(GL_CCW);
		}

		static inline void CullBackFace()
		{
			glCullFace(GL_BACK);
		}

		static inline void CullFrontFace()
		{
			glCullFace(GL_FRONT);
		}

		static inline void EnableWireframe()
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}

		static inline void DisableBlending()
		{
			glDisable(GL_BLEND);
		}

		static inline void DisableFaceCulling()
		{
			glDisable(GL_CULL_FACE);
		}

		static inline void DisableWireframe()
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		static inline void EnableBlending()
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}

		static inline void ChangeViewPort(uint32 width, uint32 height)
		{
			glViewport(0, 0, width, height);
		}

		static inline void ChangeViewPort(uint32 start_x, uint32 start_y, uint32 width, uint32 height)
		{
			glViewport(start_x, start_y, width, height);
		}


	};
}

