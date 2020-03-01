#pragma once
#include "OpenGL/OpenGlRenderer.h"
#include "OpenGL/RenderCommands.h"
#include "GPUCompute.h"
#include "Engine/Input.h"


namespace cm
{
	class CameraController
	{
	public:
		Camera main_camera;
	public:
		CameraController();
		CameraController(Camera cam);
		~CameraController();
		void CameraRotate(real delta_pitch, real delta_yaw);
		void CameraMovement(real delta_time);
		void UpdateCamera(float delta_time);
		Ray RayFromCamera(const Vec2 &mouse_position, const Vec2 &window_dimenions);


	};
	
	//class Informer
	//{
	//public:
	//	RenderShaders *render_shaders;
	//	StandardShaders *stander_shaders;
	//public:
	//	Informer();
	//	Informer(*render_shaders);
	//	
	//};

	//class Renderer
	//{

	//public:
	//	RenderShaders render_shaders;
	//	StandardShaders standard_shaders;

	//	// @TODO: Window struct
	//	uint32 WINDOW_WIDTH;
	//	uint32 WINDOW_HEIGHT;

	//	World render_world;
	//	CubeMap default_skybox;
	//	StandardMeshes standard_meshes;
	//	GPUGaussienCompute gaussien_blur_compute;

	//	FrameBuffer frame_post_processing;
	//	FrameBuffer frame_shadow_map;
	//	FrameBuffer frame_g_buffer;
	//public:
	//	Renderer();
	//	Renderer(RenderShaders render_shaders, StandardShaders standard_shaders);

	//	void Render();
	//	void Render(const World &world);
	//	
	//private:

	//	void ShadowPass(const World &world, Mat4 *light_space_matrix);
	//	void GBufferPass(const World &world);
	//	void DeferedPass(const World &world);
	//	void ForwardPass(const World &world);
	//	void PostProcessingPass(const World &world);

	//	void DrawSkyBox();

	//	void InitShaders();
	//	void InitSkyBox();
	//	void InitFrameBuffers();
	//	void InitStandardMeshes();

	//};

}