#pragma once
#include "Core.h"
#include "OpenGL/OpenGl.h"
#include "Math/CosmicMath.h"
#include "Math/CosmicGeometry.h"

namespace cm
{
	class Material
	{
	public:
		Vec3 diffuse = Vec3(1);
		Vec3 emssive = Vec3(1);
		real metalness = 0;
		real roughness = 0;

		
		// @TODO: These should be more like handels. But for my case I'm just assuming we never free any textures
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

		bool shadow_pass = true;
		bool defferd_pass = true;
		bool ssr = true;

		// @NOTE: SSAO
		bool ssao = false;
		bool ssao_changed = false;
		int32 ssao_kernel_size = 32; 
		real32 ssao_radius = 1.5;
		real32 ssao_bias = 0.025;
		real32 ssao_strength = 1.2;		
		
		// @NOTE: FXAA
		bool fxaa = false;
		real32 fxaa_span_max = 12.0f;
		real32 fxaa_dir_min = 0.1f;
		real32 fxaa_dir_reduc = 1 / 8.0f;
		
		// @NOTE: Bloom
		bool bloom = false;
		BloomBlurDependance bloom_dependance = BloomBlurDependance::Independent;
		real32 bloom_threshold = 1.0f;
		int32 bloom_kernel_size = 11;
		
		// @NOTE: Vigentte 
		bool vigentte = false;
		real32 vigentte_outer_raduis = 0.65;
		real32 vigentte_inner_raduis = 0.4;
		real32 vigentte_intensity = 0.6;

		// @NOTE: Tonemapping
		TonemapMethod tonemapping = TonemapMethod::Reinhard;
		real32 post_processing_exposure = 1.0;
	};
	   

#define RENDERFLAG_CAST_SHADOWS BIT(1)
#define RENDERFLAG_HAS_ANIMATION BIT(2)
#define RENDERFLAG_HAS_BLENDING BIT(3)

	class WorldObject
	{
	public:				
		//************************************
		// Method:    GetMaterial
		// FullName:  cm::WorldObject::GetMaterial
		// Access:    virtual public 
		// Returns:   cm::Material *
		// Qualifier:
		//************************************
		virtual Material *GetMaterial() = 0;

		//************************************
		// Method:    GetTransform
		// FullName:  cm::WorldObject::GetTransform
		// Access:    virtual public 
		// Returns:   cm::Transform *
		// Qualifier:
		//************************************
		virtual Transform *GetTransform() = 0;
		
		//************************************
		// Method:    GetMeshForRender
		// FullName:  cm::WorldObject::GetMeshForRender
		// Access:    virtual public 
		// Returns:   cm::GLMesh *
		// Qualifier:
		//************************************
		virtual GLMesh *GetMeshForRender() = 0;

		//************************************
		// Method:    GetCollider
		// FullName:  cm::WorldObject::GetCollider
		// Access:    virtual public 
		// Returns:   cm::GeometricCollider *
		// Returns:   nullptr
		// Qualifier: 
		//************************************
		virtual GeometricCollider *GetCollider() { return nullptr; }
		



		virtual const Mat4 GetTransformMatrix() const = 0;
		virtual const uint32 GetRenderFlags() const = 0;
		
		

		virtual ~WorldObject() {};
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

		inline uint32 WorldObjectCount() { return SafeTruncateUint64(objects.size()); }

		//Lighting
		DirectionalLight dir;
		std::vector<SpotLight> spot_lights;
		std::vector<PointLight> point_lights;
	};
}
