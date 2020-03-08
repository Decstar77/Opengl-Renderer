#include "Core/Renderer.h"
namespace cm
{

	CameraController::CameraController()
	{

	}

	CameraController::CameraController(Camera cam) : main_camera(cam)
	{

	}

	CameraController::~CameraController()
	{

	}

	void CameraController::CameraRotate(real delta_pitch, real delta_yaw)
	{
		main_camera.camera_yaw += delta_yaw;
		main_camera.camera_pitch += delta_pitch;
		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (main_camera.camera_pitch > 89.0f)
		{
			main_camera.camera_pitch = 89.0f;
		}
		else if (main_camera.camera_pitch < -89.0f)
		{
			main_camera.camera_pitch = -89.0f;
		}
		Vec3 front(0, 0, 0);
		front.x = cos(deg_to_rad(main_camera.camera_yaw)) * cos(deg_to_rad(main_camera.camera_pitch));
		front.y = sin(deg_to_rad(main_camera.camera_pitch));
		front.z = sin(deg_to_rad(main_camera.camera_yaw)) * cos(deg_to_rad(main_camera.camera_pitch));
		front = Normalize(front);

		Vec3 look = main_camera.transform.position + front;

		Vec3 up(0.0f, 1.0f, 0.0f); // World Space
		Vec3 camera_reverse_direction = Normalize((look - main_camera.transform.position));
		main_camera.transform.basis.right = Normalize(Cross(camera_reverse_direction, up));
		main_camera.transform.basis.upward = Cross(main_camera.transform.basis.right, camera_reverse_direction);
		main_camera.transform.basis.forward = Normalize(Cross(main_camera.transform.basis.upward, main_camera.transform.basis.right));
	}

	void CameraController::CameraMovement(real delta_time)
	{
		float cameraSpeed = 4 * delta_time;
		Vec3 cameraPos = main_camera.transform.position;
		Basis basis = main_camera.transform.basis;
		Vec3 cameraFront = basis.forward;
		Vec3 cameraUp = basis.upward;
		Vec3 cameraRight = basis.right;

		if (Input::GetKey(KEY_Q))
		{
			cameraPos.y += cameraSpeed;
		}
		if (Input::GetKey(KEY_E))
		{
			cameraPos.y -= cameraSpeed;
		}
		if (Input::GetKey(KEY_W))
		{
			cameraPos += cameraSpeed * cameraFront;
		}
		if (Input::GetKey(KEY_S))
		{
			cameraPos -= cameraSpeed * cameraFront;
		}
		if (Input::GetKey(KEY_A))
		{
			cameraPos -= cameraRight * cameraSpeed;
		}
		if (Input::GetKey(KEY_D))
		{
			cameraPos += cameraRight * cameraSpeed;
		}
		main_camera.transform.position = cameraPos;
	}

	Ray CameraController::RayFromCamera(const Vec2 &mouse_position, const Vec2 &window_dimenions)
	{
		Vec4 normal_coords = GetNormalisedDeviceCoordinates(window_dimenions.x, window_dimenions.y, mouse_position.x, mouse_position.y);
		Vec4 view_coords = ToViewCoords(main_camera.projection_matrix, normal_coords);
		// This -1 ensure we a have something pointing in to the screen
		view_coords = Vec4(view_coords.x, view_coords.y, -1, 0);
		Vec3 world_coords = ToWorldCoords(main_camera.view_matrix, view_coords);

		Ray ray;
		ray.origin = main_camera.transform.position;
		ray.direction = Normalize(Vec3(world_coords.x, world_coords.y, world_coords.z));
		return ray;
	}

	void CameraController::UpdateCamera(float delta_time)
	{
		CameraMovement(delta_time);
		Vec3 look = main_camera.transform.position + main_camera.transform.basis.forward;
		main_camera.view_matrix = LookAt(main_camera.transform.position, look, Vec3(0, 1, 0));
	}

	Renderer::Renderer()
	{
		
	}

	Renderer::Renderer(RenderShaders render_shaders, StandardShaders standard_shaders) : render_shaders(render_shaders), standard_shaders(standard_shaders)
	{
		
	}

	void Renderer::Render(const World &world)
	{
		RenderCommands::ClearColourBuffer();
		RenderCommands::ClearDepthBuffer();
		RenderCommands::Clear(Colour(0, 1, 0, 1));

		float near_plane = 1.0f, far_plane = 10.f;
		float rect = 10;
		Mat4 lightProjection = Orthographic(-rect, rect, rect, -rect, near_plane, far_plane);
		//Mat4 light_view = LookAt(cpos, cpos + Normalize(target), Vec3(0, 1, 0));
		Mat4 light_view = LookAt(Vec3(-2.0f, 4.0f, -1.0f), Vec3(0), Vec3(0, 1, 0));
		Mat4 light_space_matrix = light_view * lightProjection;
		
		if (render_settings.shadow_pass)
		{
			ShadowPass(world, &light_space_matrix);
		}
		if (render_settings.defferd_pass)
		{
			GBufferPass(world);
		}
		
		BindFrameBuffer(frame_post_processing);
		RenderCommands::ClearColourBuffer();
		RenderCommands::ClearDepthBuffer();

		DrawSkyBox();

		if (render_settings.defferd_pass)
		{
			DeferedPass(world);
		}

		ForwardPass(world);
		
		UnbindFrameBuffer();

		PostProcessingPass(world);
		
	}

	void Renderer::ShadowPass(const World &world, Mat4 *light_space_matrix)
	{
		BindFrameBuffer(frame_shadow_map);
		BindShader(render_shaders.depth_test_shader);

		RenderCommands::ChangeViewPort(frame_shadow_map.depth_texture_attachment.config.width, 
			frame_shadow_map.depth_texture_attachment.config.height);
		RenderCommands::ClearDepthBuffer();
		RenderCommands::CullFrontFace();
		
		ShaderSetMat4(render_shaders.depth_test_shader, "lightSpaceMatrix", light_space_matrix->arr); // It's in ubo

		RenderWorld(&render_shaders.depth_test_shader, nullptr, world);

		UnbindFrameBuffer();
		
		RenderCommands::CullBackFace();
		RenderCommands::ChangeViewPort(WINDOW_WIDTH, WINDOW_HEIGHT);
	}

	void Renderer::GBufferPass(const World &world)
	{

		BindFrameBuffer(frame_g_buffer);
		BindShader(render_shaders.g_buffer_shader);
		RenderCommands::ClearBuffers();

		RenderWorld(&render_shaders.g_buffer_shader, nullptr, world);

		UnbindFrameBuffer();
	}

	void Renderer::DeferedPass(const World &world)
	{
		// @TODO: Anything lolz
		// @TODO: SSAO
		// @TODO: SSRF
	}

	void Renderer::ForwardPass(const World &world)
	{
		BindShader(render_shaders.render_shader);


		if (render_settings.shadow_pass)
		{
			ShaderBindTexture(render_shaders.render_shader, frame_shadow_map.depth_texture_attachment, 0, "shadow_map");
		}

		RenderWorld(&render_shaders.render_shader, &render_shaders.render_batch_shader, world);		
	}

	void Renderer::PostProcessingPass(const World &world)
	{
		// @TODO: Create a better solution for the blur 
		//bloom_blur.GPUGaussienBlur();
		BindShader(render_shaders.post_processing_shader);
		
		ShaderSetFloat(render_shaders.post_processing_shader, "exposure", 1.0f);
		ShaderBindTexture(render_shaders.post_processing_shader, frame_post_processing.colour0_texture_attachment, 0, "scene_texture");
		//ShaderBindTexture(render_shaders.post_processing_shader, *bloom_blur.texture_to_blur, 1, "bloom_texture");

		RenderMesh(render_shaders.post_processing_shader, standard_meshes.quad);
	}


	void Renderer::DrawSkyBox()
	{
		// @TODO: Fill in ubos
		RenderCommands::DisableFaceCulling();
		BindShader(render_shaders.skybox_shader);
		ShaderSetMat4(render_shaders.skybox_shader, "projection", camera->main_camera.projection_matrix.arr);
		ShaderSetMat4(render_shaders.skybox_shader, "view", camera->main_camera.view_matrix.arr);
		RenderMesh(render_shaders.skybox_shader, standard_meshes.cube); // It has texture coords
		RenderCommands::EnableFaceCulling();
	}

	void Renderer::InitShaders()
	{
		// @TODO: Fix up AssetLoader for linking errors
		//Shader cubemap_shader = CreateShader(ReadFile("shaders/Rendering/skybox_vert.glsl"), ReadFile("shaders/Rendering/skybox_frag.glsl"));
		//cubemap_shader.name = "cubemap_shader";

		//Shader pbr_nomaps_shader = CreateShader(ReadFile("shaders/Rendering/pbr_nomaps_vert.glsl"), ReadFile("shaders/Rendering/pbr_nomaps_frag.glsl"));
		//pbr_nomaps_shader.name = "pbr_nomaps";

		//Shader pbr_nomaps_batch_shader = CreateShader(ReadFile("shaders/Rendering/pbr_nomaps_batch_vert.glsl"), ReadFile("shaders/Rendering/pbr_nomaps_frag.glsl"));
		//pbr_nomaps_batch_shader.name = "pbr_nomaps_batch_shader";

		//Shader post_processing_shader = CreateShader(ReadFile("shaders/Rendering/post_processing_vert.glsl"), ReadFile("shaders/Rendering/post_processing_frag.glsl"));
		//post_processing_shader.name = "post_processing_shader";

		//Shader simple_shadow_map_shader = CreateShader(ReadFile("shaders/Rendering/simple_shadow_map_vert.glsl"), ReadFile("shaders/Rendering/simple_shadow_map_frag.glsl"));
		//simple_shadow_map_shader.name = "simple_shadow_map_vert";

		//Shader gpu_gaussian_blur_shader = CreateShader(ReadFile("shaders/Rendering/GPU_gaussian_blur_vert.glsl"), ReadFile("shaders/Rendering/GPU_gaussian_blur_frag.glsl"));
		//gpu_gaussian_blur_shader.name = "gaussian_blur";

		//Shader g_buffer_shader = CreateShader(ReadFile("shaders/Rendering/g_buffer_vert.glsl"), ReadFile("shaders/Rendering/g_buffer_frag.glsl"));
		//g_buffer_shader.name = "g_buffer_shader ";
	}

	void Renderer::InitFrameBuffers()
	{
		FrameBuffer post_processing;
		CreateFrameBuffer(&post_processing);
		BindFrameBuffer(post_processing);

		TextureConfig post_processing_colour_buffers_config;
		post_processing_colour_buffers_config.height = WINDOW_HEIGHT;
		post_processing_colour_buffers_config.width = WINDOW_WIDTH;
		post_processing_colour_buffers_config.data_type = GL_FLOAT;
		post_processing_colour_buffers_config.texture_format = GL_RGBA16F;
		post_processing_colour_buffers_config.wrap_s_mode = GL_CLAMP_TO_EDGE;
		post_processing_colour_buffers_config.wrap_t_mode = GL_CLAMP_TO_EDGE;
		post_processing_colour_buffers_config.wrap_r_mode = GL_CLAMP_TO_EDGE;

		post_processing.colour0_texture_attachment.config = post_processing_colour_buffers_config;
		post_processing.colour1_texture_attachment.config = post_processing_colour_buffers_config;

		CreateTexture(&post_processing.colour0_texture_attachment, nullptr);
		CreateTexture(&post_processing.colour1_texture_attachment, nullptr);

		post_processing.render_attchment.width = WINDOW_WIDTH;
		post_processing.render_attchment.height = WINDOW_HEIGHT;

		FrameBufferAddColourAttachtments(&post_processing);
		FrameBufferAddColourAttachtments(&post_processing);
		FrameAddBufferRenderAttachtment(&post_processing);

		Assert(CheckFrameBuffer(post_processing));

		FrameBuffer shadow_map;
		shadow_map.depth_texture_attachment.config.texture_format = GL_DEPTH_COMPONENT32;
		shadow_map.depth_texture_attachment.config.pixel_format = GL_DEPTH_COMPONENT;
		shadow_map.depth_texture_attachment.config.min_filter = GL_NEAREST;
		shadow_map.depth_texture_attachment.config.mag_filter = GL_NEAREST;
		shadow_map.depth_texture_attachment.config.data_type = GL_FLOAT;
		shadow_map.depth_texture_attachment.config.wrap_s_mode = GL_CLAMP_TO_BORDER;
		shadow_map.depth_texture_attachment.config.wrap_t_mode = GL_CLAMP_TO_BORDER;
		shadow_map.depth_texture_attachment.config.width = 1024;
		shadow_map.depth_texture_attachment.config.height = 1024;

		CreateTexture(&shadow_map.depth_texture_attachment, nullptr);
		TextureSetBorder(&shadow_map.depth_texture_attachment, Vec4(1).arr);

		CreateFrameBuffer(&shadow_map);
		FrameBufferAddDepthAttachments(&shadow_map);

		Assert(CheckFrameBuffer(shadow_map));

		FrameBuffer g_buffer;

		g_buffer.colour0_texture_attachment.config.height = WINDOW_HEIGHT;
		g_buffer.colour0_texture_attachment.config.width = WINDOW_WIDTH;
		g_buffer.colour0_texture_attachment.config.data_type = GL_FLOAT;
		g_buffer.colour0_texture_attachment.config.texture_format = GL_RGBA16F;
		g_buffer.colour0_texture_attachment.config.wrap_s_mode = GL_CLAMP_TO_EDGE;
		g_buffer.colour0_texture_attachment.config.wrap_t_mode = GL_CLAMP_TO_EDGE;
		g_buffer.colour0_texture_attachment.config.wrap_r_mode = GL_CLAMP_TO_EDGE;

		g_buffer.colour1_texture_attachment.config.height = WINDOW_HEIGHT;
		g_buffer.colour1_texture_attachment.config.width = WINDOW_WIDTH;
		g_buffer.colour1_texture_attachment.config.data_type = GL_FLOAT;
		g_buffer.colour1_texture_attachment.config.texture_format = GL_RGBA16F;
		g_buffer.colour1_texture_attachment.config.wrap_s_mode = GL_CLAMP_TO_EDGE;
		g_buffer.colour1_texture_attachment.config.wrap_t_mode = GL_CLAMP_TO_EDGE;
		g_buffer.colour1_texture_attachment.config.wrap_r_mode = GL_CLAMP_TO_EDGE;

		g_buffer.colour2_texture_attachment.config.height = WINDOW_HEIGHT;
		g_buffer.colour2_texture_attachment.config.width = WINDOW_WIDTH;
		g_buffer.colour2_texture_attachment.config.data_type = GL_UNSIGNED_BYTE;
		g_buffer.colour2_texture_attachment.config.texture_format = GL_RGBA;
		g_buffer.colour2_texture_attachment.config.wrap_s_mode = GL_CLAMP_TO_EDGE;
		g_buffer.colour2_texture_attachment.config.wrap_t_mode = GL_CLAMP_TO_EDGE;
		g_buffer.colour2_texture_attachment.config.wrap_r_mode = GL_CLAMP_TO_EDGE;

		g_buffer.render_attchment.width = WINDOW_WIDTH;
		g_buffer.render_attchment.height = WINDOW_HEIGHT;

		CreateTexture(&g_buffer.colour0_texture_attachment, nullptr);
		CreateTexture(&g_buffer.colour1_texture_attachment, nullptr);
		CreateTexture(&g_buffer.colour2_texture_attachment, nullptr);
		CreateFrameBuffer(&g_buffer);
		FrameBufferAddColourAttachtments(&g_buffer);
		FrameAddBufferRenderAttachtment(&g_buffer);

		Assert(CheckFrameBuffer(g_buffer));

		frame_post_processing = post_processing;
		frame_shadow_map = shadow_map;
		frame_g_buffer = g_buffer;

	}

	void Renderer::InitStandardMeshes()
	{

	}

	void Renderer::InitSkyBox()
	{
		CubeMap default_skybox0;
		default_skybox0.config.data_type = GL_FLOAT;
		default_skybox0.config.texture_format = GL_RGBA16;
		default_skybox0.config.min_filter = GL_LINEAR;
		default_skybox0.config.mag_filter = GL_LINEAR;
		default_skybox0.config.wrap_r_mode = GL_CLAMP_TO_EDGE;
		default_skybox0.config.wrap_s_mode = GL_CLAMP_TO_EDGE;
		default_skybox0.config.wrap_t_mode = GL_CLAMP_TO_EDGE;
		default_skybox0.config.width = 512;
		default_skybox0.config.height = 512;
		CreateCubeMap(&default_skybox0, nullptr);
		default_skybox = default_skybox0;
	}

}