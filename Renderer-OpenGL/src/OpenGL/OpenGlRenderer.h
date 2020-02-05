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
		//.. Add as needed;
	};

	// @THINK: This could be a OpenGLVertex type thing where we 
	//		 : Standardize all out vert shaders input
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
		 // @NOTE: We will now have to calculate the matrix on the render thread
		Transform transform;
	};

	struct Batch
	{
		VertexArray vao;
		IndexBuffer copied_ibo;
		DynaArray<Mat4> transforms;
	};

	struct PointLight
	{
		Vec3 LightColour;
		Vec3 LightPos;
	};

	struct Camera
	{
		float  camera_yaw = -90.0f;
		float  camera_pitch = 0.0f;

		Transform transform;
		Vec3 target;
		Mat4 view_matrix;
		Mat4 projection_matrix;
	};

	struct RenderSettings
	{
		//bool Shadows = true;
		//bool Forward = true;
	};

	struct World
	{
		Camera camera;
		RenderSettings render_settings;		
		DynaArray<Batch> batches;
		DynaArray<Actor> actors;
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