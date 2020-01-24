#pragma once
#include "glew.h"
#include "Core.h"
#include "Math/CosmicMath.h" //YEET OUT Colour can be an uint32



namespace cm
{
	class RenderCommands
	{
	public:
		static inline void EnableMulitSampling()
		{
			glEnable(GL_MULTISAMPLE);
		}

		static inline void Clear(Vec4 colour)
		{
			glClearColor(colour.x, colour.y, colour.z, colour.w);
		}

		static inline void EnableDepthBuffer()
		{
			glEnable(GL_DEPTH_TEST);

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

		static inline void ClearBuffers()
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
			glCullFace(GL_BACK);
			glFrontFace(GL_CCW);
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

	};
}

