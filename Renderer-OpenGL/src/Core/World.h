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

		Shader *shader;
	};

	struct PointLight 
	{
		Vec3 light_colour;
		Vec3 light_position;
	};

	struct DirectionalLight 
	{
		Vec3 light_colour;
		Vec3 direction;
	};

	struct SpotLight 
	{
		Vec3 light_colour;
		Vec3 light_position;
		// @NOTE: direction.w = raduis
		Vec4 direction; 
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
		virtual void SetTextures(Shader *shader) = 0;
		virtual void SetMaterialValues(Shader *shader) = 0;
		virtual void SetTransformValues(Shader *shader) = 0;
		virtual const GLMesh &GetMeshForRender() = 0;
	};

	struct World 
	{
		RenderSettings render_settings;
		std::vector<WorldObject*> objects;
		

		//Lighting
		DirectionalLight dir;
		std::vector<SpotLight> spot_lights;
		std::vector<PointLight> point_lights;
	};


	void RenderWorld(Shader *shader, Shader *batch_shader, const World &world);
}
