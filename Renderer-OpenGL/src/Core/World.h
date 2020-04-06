#pragma once
#include "Core.h"
#include "OpenGL/OpenGl.h"
#include "Math/CosmicMath.h"
namespace cm
{
	class Material
	{
	public:
		Vec3 diffuse = Vec3(1);
		Vec3 emssive = Vec3(1);
		real metalness = 0;
		real roughness = 0;

		// @NOTE: These should be more like handels. But for my case I'm just assuming we never free any textures
		Texture *diffuse_texture = nullptr;
		Texture *normal_texture = nullptr;
		Texture *occlusion_roughness_metallic = nullptr;
		Texture *emssive_texture = nullptr;
	};

	struct PointLight // Out
	{
		Vec3 light_colour;
		Vec3 light_position;
	};

	struct DirectionalLight // Out
	{
		Vec3 light_colour;
		Vec3 direction;
	};

	struct SpotLight // Out
	{
		Vec3 light_colour;
		Vec3 light_position;
		Vec4 direction; // w = raduis
	};

	struct RenderSettings
	{
		bool shadow_pass = true;
		bool defferd_pass = true;
		bool bloom = true;
		bool ssao = true;
		bool ssr = true;
	};
	   	 
	class WorldObject
	{
	public:
		virtual void SetMaterialValues(Shader *shader, Texture identity_texture) = 0;
		virtual void SetTransformValues(Shader *shader) = 0;
		virtual const GLMesh &GetMeshForRender() = 0;
	};

	class Actor : public WorldObject
	{
	public:
		Transform transform;
		Material material;
		GLMesh mesh;

		virtual void SetMaterialValues(Shader *shader, Texture identity_texture) override;

		virtual void SetTransformValues(Shader *shader) override;

		virtual const GLMesh & GetMeshForRender() override;

	};

	struct World // out
	{
		RenderSettings render_settings;
		DynaArray<WorldObject*> objects;
		

		//Lighting
		DirectionalLight dir;
		DynaArray<SpotLight> spot_lights;
		DynaArray<PointLight> point_lights;
	};


	void RenderWorld(Shader *shader, Shader *batch_shader, const World &world);
}
