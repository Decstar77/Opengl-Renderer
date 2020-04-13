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
		bool bloom = true;
		bool ssao = true;
		bool ssr = true;
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

	struct World 
	{
		RenderSettings render_settings;
		std::vector<WorldObject*> objects;
		

		//Lighting
		DirectionalLight dir;
		std::vector<SpotLight> spot_lights;
		std::vector<PointLight> point_lights;
	};

	class EquirectangularToCubemap
	{
	private:
		FrameBuffer eqi_frame;
		Shader eqi_shader;
		bool created = false;

		Mat4 projection = Perspective(90.0f, 1.0f, 0.1f, 10.0f);
		Mat4 views[6] =
		{
		   LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f,  0.0f,  0.0f), Vec3(0.0f, -1.0f,  0.0f)),
		   LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(-1.0f,  0.0f,  0.0f),Vec3(0.0f, -1.0f,  0.0f)),
		   LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f,  1.0f,  0.0f), Vec3(0.0f,  0.0f,  1.0f)),
		   LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, -1.0f,  0.0f), Vec3(0.0f,  0.0f, -1.0f)),
		   LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f,  0.0f,  1.0f), Vec3(0.0f, -1.0f,  0.0f)),
		   LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f,  0.0f, -1.0f), Vec3(0.0f, -1.0f,  0.0f))
		};

	public:		
		void Create();
		void Convert(const Texture &src, CubeMap *dst);
		void Free();	
		
	};


	void RenderWorld(Shader *shader, Shader *batch_shader, const World &world);
}
