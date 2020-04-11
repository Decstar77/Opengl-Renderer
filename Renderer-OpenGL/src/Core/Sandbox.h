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
		Transform transform;
		Material material;
		GLMesh mesh;

		virtual void SetTextures(Shader *shader) override;
		virtual void SetMaterialValues(Shader *shader) override;
		virtual void SetTransformValues(Shader *shader) override;
		virtual const GLMesh &GetMeshForRender() override;

	};

	class AnimatedActor : public  WorldObject
	{
	public:
		GLMesh mesh;
		Material material;
		Transform transform;

		AnimationController animation_controller;

		virtual void SetTextures(Shader *shader) override;
		virtual void SetMaterialValues(Shader *shader) override;
		virtual void SetTransformValues(Shader *shader) override;
		virtual const GLMesh &GetMeshForRender() override;

	};







}






