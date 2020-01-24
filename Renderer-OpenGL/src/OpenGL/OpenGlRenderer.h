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

	struct Actor
	{
		Mesh mesh;
		Material material;
		Transform transform; // @NOTE: We will now have to calculate the matrix on the render thread
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
		DynaArray<Shader> shaders;
		DynaArray<Actor> actors;
		DynaArray<PointLight> point_lights;
	};

	void CreateBatch(Batch *batch, const VertexBuffer &vbo_to_batch, const IndexBuffer &ibo_of_vbo);
	void FreeBatch(Batch *batch);
	void RenderBatch(const Shader &shader, const Batch &batch);
	void RenderMesh(const Shader &shader, const Mesh &mesh);
	void RenderWorld(Shader *shader, const World &world);
}