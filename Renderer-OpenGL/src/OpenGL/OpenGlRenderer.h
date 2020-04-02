#pragma once
#include "Core.h"
#include "OpenGL.h"
namespace cm
{
	struct Material
	{
		Vec3 diffuse;
		Texture diffuse_texture;
		Vec3 metalness;
		Texture metalness_texture;
		Vec3 roughness;
		Texture roughness_texture;
		Vec3 emssive;
		Texture emssive_texture;
		uint32 ShaderIndex;
	};

	struct Mesh
	{
		VertexArray vao;
		IndexBuffer ibo;
	};
		
	struct StandardMeshes 
	{
		Mesh quad;
		Mesh plane;
		Mesh cube;
		Mesh sphere;
		Mesh cone;
	};
	
	struct StandardShaders
	{
		Shader gpu_gaussian_blur;
		Shader simple_blur;
		//.. Add as needed;
	};

	struct RenderShaders
	{
		Shader forward_render_shader;
		Shader forward_render_batch_shader;
		Shader deferred_render_shader;
		Shader deferred_render_batch_shader;
		Shader skybox_shader;
		Shader post_processing_shader;
		Shader depth_test_shader;
		Shader g_buffer_shader;		
		Shader ssao_gbuffer_shader;
		Shader ssao_shader;
		Shader debug_shader;
		Shader debug_mesh_shader;
	};

	struct Vertex
	{
		Vec3 position;
		Vec3 normal;
		Vec3 texture_coord;
		Vec3 colour;
	};

	struct Edge
	{
		Vertex vertices[2] = {};
		real32 length = 0;
	};

	struct Triangle
	{
		real32 area;
		Vec3 normal;
		Vertex vertices[3];
		Edge edges[2];
	};


	struct Actor
	{
		Mesh mesh;
		//Material material;
		Transform transform;
	};

	struct Batch
	{
		VertexArray vao;
		IndexBuffer ibo;
		DynaArray<Mat4> transforms;
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
		Vec4 direction; // w = raduis
	};

	struct Camera
	{
		real  camera_yaw = -90.0f;
		real  camera_pitch = 0.0f;

		Transform transform;
		Vec3 target;
		Mat4 view_matrix;
		Mat4 projection_matrix;
	};

	struct RenderSettings
	{
		bool shadow_pass = true;
		bool defferd_pass = true;
		bool bloom = true;
		bool ssao = true;
		bool ssr = true;
	};

	struct World
	{
		RenderSettings render_settings;		
		DynaArray<Batch> batches;
		DynaArray<Actor> actors;

		//Lighting
		DirectionalLight dir;
		DynaArray<SpotLight> spot_lights;		
		DynaArray<PointLight> point_lights;
	};
		
	
	struct PostProcessingStack
	{
		FrameBuffer post_processing_fbo;		
		//...settings...
	};

	void CreateBatch(Batch *batch, const VertexBuffer &vbo_to_batch, const IndexBuffer &ibo_of_vbo);
	void FreeBatch(Batch *batch);
	void RenderBatch(const Shader &shader, const Batch &batch);
	void RenderMesh(const Shader &shader, const Mesh &mesh);
	void RenderWorld(Shader *shader, Shader *batch_shader, const World &world);

}