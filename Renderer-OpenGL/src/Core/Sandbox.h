#pragma once
#include "Core.h"
#include "Utility.h"
#include "Animation.h"
#include "EditableMesh.h"
#include "World.h"

namespace cm
{
	class AnimatedActor : public  WorldObject
	{
	public:
		uint32 render_flags = RENDERFLAG_CAST_SHADOWS | RENDERFLAG_HAS_ANIMATION;
		GLMesh mesh;
		Material material;
		Transform transform;
		AnimationController animation_controller;

		virtual Material *GetMaterial() override;
		virtual Transform *GetTransform() override;
		virtual GLMesh *GetMeshForRender() override;
		virtual GeometricCollider *GetCollider() override;
		virtual uint32 GetRenderFlags() const override;
	};



}






