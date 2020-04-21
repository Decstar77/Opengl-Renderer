#pragma once
#include "Core.h"
#include "OpenGl/RenderCommands.h"
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

		Shader *forward_shader	= nullptr;
		Shader *gbuffer_shader	= nullptr;
		Shader *shadow_shader = nullptr;
		Shader *instance_shader = nullptr;
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
		bool ssr = true;


		bool ssao = false;
		bool ssao_changed = false;
		int32 ssao_kernel_size = 32; 
		real32 ssao_radius = 1.5;
		real32 ssao_bias = 0.025;
		real32 ssao_strength = 1.2;

		enum TonemapMethod
		{
			// @NOTE: These vaules are important because they are cast to the shader
			Reinhard = 0,
			Uncharted = 1,
			Filmic = 2
		};

		enum  BloomBlurDependance
		{
			Dependent = 0,
			Independent = 1
		};

		TonemapMethod tonemapping = TonemapMethod::Reinhard;

		bool fxaa = false;
		real32 fxaa_span_max = 12.0f;
		real32 fxaa_dir_min = 0.1f;
		real32 fxaa_dir_reduc = 1 / 8.0f;
		
		bool bloom = true;
		BloomBlurDependance bloom_dependance = BloomBlurDependance::Independent;
		real32 bloom_threshold = 1.0f;
		real32 bloom_downsample_mul = 0.5f;
		int32 bloom_kernel_size = 11;
		int32 bloom_blur_iterations = 1;

		bool vigentte = false;
		real32 vigentte_outer_raduis = 0.65;
		real32 vigentte_inner_raduis = 0.4;
		real32 vigentte_intensity = 0.6;

		real32 post_processing_exposure = 1.0;
	};
	   

#define RENDERFLAG_CAST_SHADOWS BIT(1)
#define RENDERFLAG_HAS_ANIMATION BIT(2)
#define RENDERFLAG_HAS_BLENDING BIT(3)

	class WorldObject
	{
	public:				
		virtual const Material GetMaterial() const = 0;
		virtual const Transform GetTransfrom() const = 0;
		virtual const Mat4 GetTransformMatrix() const = 0;
		virtual const uint32 GetRenderFlags() const = 0;
		virtual const GLMesh GetMeshForRender() const = 0;	
	};

	class World 
	{
	private:
		std::vector<WorldObject*> animated_objects;
		std::vector<WorldObject*> static_objects;

	public:
		RenderSettings render_settings;
		std::vector<WorldObject*> objects;
		

		void RegisterWorldObject(PointLight *point_light);
		void RegisterWorldObject(WorldObject *object);
		void DeregisterWorldObject(WorldObject *object);

		//Lighting
		DirectionalLight dir;
		std::vector<SpotLight> spot_lights;
		std::vector<PointLight> point_lights;
	};



	void RenderWorld(Shader *shader, Shader *batch_shader, const World &world);
}
