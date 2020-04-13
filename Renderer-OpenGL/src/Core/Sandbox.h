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
		GLMesh mesh;

		virtual const GLMesh GetMeshForRender() const override;
		virtual const Material GetMaterial() const override;
		virtual const Transform GetTransfrom() const override;
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

		virtual const Material GetMaterial() const override;
		virtual const Transform GetTransfrom() const override;
		virtual const Mat4 GetTransformMatrix() const override;
		virtual const uint32 GetRenderFlags() const override;
		virtual const GLMesh GetMeshForRender() const override;
	};







}






