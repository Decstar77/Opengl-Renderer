#pragma once
#include "Core.h"
#include "Utility.h"
#include "Animation.h"
#include "EditableMesh.h"
#include "World.h"

namespace cm
{
	class Actor : public WorldObject
	{
	public:
		uint32 render_flags = RENDERFLAG_CAST_SHADOWS;
		Transform transform;
		Material material;
		OBB obb;
		GLMesh mesh;

		virtual GLMesh *GetMeshForRender() override;
		virtual Material *GetMaterial() override;
		virtual Transform *GetTransform() override;
		virtual GeometricCollider *GetCollider() override;
		virtual const Mat4 GetTransformMatrix() const override;
		virtual const uint32 GetRenderFlags() const override;
	};

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
		virtual const Mat4 GetTransformMatrix() const override;
		virtual const uint32 GetRenderFlags() const override;
	};

	Actor CreateActorCube();




}






