#include "Core/Renderer.h"
namespace cm
{
	WorldRenderer::WorldRenderer()
	{
		
	}

	WorldRenderer::WorldRenderer(RenderShaders render_shaders, StandardShaders standard_shaders) : render_shaders(render_shaders), standard_shaders(standard_shaders)
	{
		
	}

	void WorldRenderer::Render(const World &world)
	{
		RenderCommands::ClearColourBuffer();
		RenderCommands::ClearDepthBuffer();
		RenderCommands::Clear(Colour(0, 1, 0, 1));

		float near_plane = 1.f, far_plane = 10.f;
		float rect = 8;
		Mat4 lightProjection = Orthographic(-rect, rect, rect, -rect, near_plane, far_plane);
		//Mat4 light_view = LookAt(cpos, cpos + Normalize(target), Vec3(0, 1, 0));
		Mat4 light_view = LookAt(Vec3(-2.0f, 4.0f, -1.0f), Vec3(0), Vec3(0, 1, 0));
		Mat4 light_space_matrix = light_view * lightProjection;
		
		// @NOTE: We meant store all the shadow maps, eviorment maps etc if the first texture slots and then the others later
		

		if (render_settings.shadow_pass)
		{
			ShadowPass(world, &light_space_matrix);
		}		
		
		if (render_settings.ssao)
		{
			SSAOPass(world);
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
			//
			//DeferedPass(world);
			debug();
		}

		ForwardPass(world);
		

		UnbindFrameBuffer();

		PostProcessingPass(world);

	}

	void WorldRenderer::ShadowPass(const World &world, Mat4 *light_space_matrix)
	{
		BindFrameBuffer(frame_shadow_map);
		BindShader(render_shaders.depth_test_shader);

		RenderCommands::ChangeViewPort(frame_shadow_map.colour0_texture_attachment.config.width, 
			frame_shadow_map.colour0_texture_attachment.config.height);
		RenderCommands::ClearColourBuffer();
		RenderCommands::CullFrontFace();
		ShaderSetMat4(&render_shaders.depth_test_shader, "lightSpaceMatrix", light_space_matrix->arr); // It's in ubo

		RenderWorld(&render_shaders.depth_test_shader, nullptr, world);

		UnbindFrameBuffer();
		RenderCommands::CullBackFace();
		
		RenderCommands::ChangeViewPort(WINDOW_WIDTH, WINDOW_HEIGHT);
	}

	void WorldRenderer::GBufferPass(const World &world)
	{
		BindFrameBuffer(frame_g_buffer);
		BindShader(render_shaders.g_buffer_shader);
		RenderCommands::Clear(Vec4(0, 0, 0, 0));
		RenderCommands::ClearBuffers();
		
		RenderWorld(&render_shaders.g_buffer_shader, nullptr, world);

		UnbindFrameBuffer();
	}

	void WorldRenderer::DeferedPass(const World &world)
	{
		BindShader(render_shaders.deferred_render_shader);
		
		ShaderBindTexture(render_shaders.deferred_render_shader, frame_g_buffer.colour0_texture_attachment, 0, "position_map");
		ShaderBindTexture(render_shaders.deferred_render_shader, frame_g_buffer.colour1_texture_attachment, 1, "normal_map");
		ShaderBindTexture(render_shaders.deferred_render_shader, frame_g_buffer.colour2_texture_attachment, 2, "colour_map");
		ShaderBindTexture(render_shaders.deferred_render_shader, frame_ssao_blured.colour0_texture_attachment, 3, "ssao_map");
		RenderMesh(render_shaders.deferred_render_shader, StandardMeshes::quad);
	}

	void WorldRenderer::ForwardPass(const World &world)
	{		
		if (render_settings.shadow_pass)
		{
			//ShaderBindTexture(render_shaders.forward_render_shader, frame_shadow_map.colour0_texture_attachment, 0, "shadow_map");
		}

		for (int32 i = 0; i < world.objects.size(); i++)
		{
			WorldObject *a = world.objects[i];
			Transform transform = a->GetTransfrom();
			Material mat = a->GetMaterial();
			Mat4 transform_matrix = a->GetTransformMatrix();
			uint32 flags = a->GetRenderFlags();
			
			Shader forward_shader = *mat.forward_shader;

			BindShader(forward_shader);

			
			//ShaderSetFloat(&forward_shader, "material_roughness", mat.roughness);
			//ShaderSetFloat(&forward_shader, "material_metalness", mat.metalness);

			ShaderSetMat4(&forward_shader, "model", transform_matrix.arr);

			RenderMesh(forward_shader, a->GetMeshForRender());
		}
	}

	void WorldRenderer::PostProcessingPass(const World &world)
	{
		// @TODO: Create a better solution for the blur 
		//bloom_blur.GPUGaussienBlur();		
		BindShader(render_shaders.post_processing_shader);
		
		ShaderSetFloat(&render_shaders.post_processing_shader, "exposure", 1.0f);
		ShaderBindTexture(render_shaders.post_processing_shader, frame_post_processing.colour0_texture_attachment, 0, "scene_texture");
		//ShaderBindTexture(render_shaders.post_processing_shader, *bloom_blur.texture_to_blur, 1, "bloom_texture");

		RenderMesh(render_shaders.post_processing_shader, StandardMeshes::quad);
	}

	void WorldRenderer::SSAOPass(const World &world)
	{
		// @STEP: Fill in the gbuffer. Note, this is all view space things.
		BindFrameBuffer(frame_g_buffer);
		RenderCommands::ClearBuffers();
		RenderWorld(&render_shaders.ssao_gbuffer_shader, nullptr, world);
		UnbindFrameBuffer();

		static bool init = false;
		static Texture noise_texture;

	
		std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // random floats between 0.0 - 1.0
		std::default_random_engine generator;
		std::vector<Vec3> ssaoKernel;
		for (unsigned int i = 0; i < 32; ++i)
		{
			Vec3 sample(
				randomFloats(generator) * 2.0 - 1.0,
				randomFloats(generator) * 2.0 - 1.0,
				randomFloats(generator)
			);
			sample = Normalize(sample);
			sample = sample * randomFloats(generator);
			float scale = (float)i / 64.0;
			//ssaoKernel.push_back(sample);

			scale = Lerp(0.1f, 1.0f, scale * scale);
			sample = sample * scale;
			ssaoKernel.push_back(sample);
		}
				
		if (!init)
		{
			std::vector<Vec3> ssaoNoise;
			for (unsigned int i = 0; i < 16; i++)
			{
				Vec3 noise(
					randomFloats(generator) * 2.0 - 1.0,
					randomFloats(generator) * 2.0 - 1.0,
					0.0f);
				ssaoNoise.push_back(noise);
			}
			init = true;
			noise_texture.config.height = 4;
			noise_texture.config.width = 4;
			noise_texture.config.data_type = GL_FLOAT;
			noise_texture.config.texture_format = GL_RGBA32F;
			noise_texture.config.min_filter = GL_NEAREST;
			noise_texture.config.mag_filter = GL_NEAREST;
			noise_texture.config.wrap_s_mode = GL_REPEAT;
			noise_texture.config.wrap_t_mode = GL_REPEAT;
			noise_texture.config.wrap_r_mode = GL_REPEAT;
			CreateTexture(&noise_texture, ssaoNoise.data());		

		}
		
		BindFrameBuffer(frame_ssao_unblured);
		RenderCommands::ClearColourBuffer();
		BindShader(render_shaders.ssao_shader);

		ShaderSetMat4(&render_shaders.ssao_shader, "projection", camera->main_camera.projection_matrix.arr);
		for (unsigned int i = 0; i < 32; ++i)
			ShaderSetVec3(&render_shaders.ssao_shader, "samples[" + std::to_string(i) + "]", ssaoKernel[i].arr);
		ShaderBindTexture(render_shaders.ssao_shader, frame_g_buffer.colour0_texture_attachment, 0, "g_position");
		ShaderBindTexture(render_shaders.ssao_shader, frame_g_buffer.colour1_texture_attachment, 1, "g_normal");
		ShaderBindTexture(render_shaders.ssao_shader, noise_texture, 2, "noise_texture");

		RenderMesh(render_shaders.ssao_shader, StandardMeshes::quad);


		BindFrameBuffer(frame_ssao_blured);
		RenderCommands::ClearColourBuffer();
		BindShader(standard_shaders.simple_blur);
		
		ShaderBindTexture(standard_shaders.simple_blur, frame_ssao_unblured.colour0_texture_attachment, 0, "texture_to_blur");		
		
		RenderMesh(standard_shaders.simple_blur, StandardMeshes::quad);

	}

	void WorldRenderer::debug()
	{
		BindShader(render_shaders.debug_mesh_shader);
		glViewport(0, 0, WINDOW_WIDTH / 4, WINDOW_HEIGHT / 4);
		ShaderBindTexture(render_shaders.debug_mesh_shader, frame_shadow_map.colour0_texture_attachment, 0, "mesh_texture");
		ShaderSetMat4(&render_shaders.debug_mesh_shader, "model", Mat4(1).arr);
		RenderMesh(render_shaders.debug_mesh_shader, StandardMeshes::quad);
		

		ShaderBindTexture(render_shaders.debug_mesh_shader, frame_ssao_blured.colour0_texture_attachment, 0, "mesh_texture");
		glViewport(WINDOW_WIDTH / 4, 0, WINDOW_WIDTH / 4, WINDOW_HEIGHT / 4);
		RenderMesh(render_shaders.debug_mesh_shader, StandardMeshes::quad);
		

		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	}

	void WorldRenderer::DrawSkyBox()
	{
		RenderCommands::DisableFaceCulling();
		
		BindShader(render_shaders.skybox_shader);

		// @TODO: Fill in ubos
		ShaderSetMat4(&render_shaders.skybox_shader, "projection", camera->main_camera.projection_matrix.arr);
		ShaderSetMat4(&render_shaders.skybox_shader, "view", camera->main_camera.view_matrix.arr);
		
		if (default_skybox.object != 0)
		{
			ShaderBindCubeMap(&render_shaders.skybox_shader, default_skybox, 0, "environmentMap");
		}

		RenderMesh(render_shaders.skybox_shader, StandardMeshes::cube); 
		
		RenderCommands::EnableFaceCulling();
	}


	void WorldRenderer::InitShaders()
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

	void WorldRenderer::InitFrameBuffers()
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

		FrameBufferBindColourAttachtments(&post_processing);
		FrameBufferBindColourAttachtments(&post_processing);
		FrameAddBufferRenderAttachtment(&post_processing);

		Assert(CheckFrameBuffer(post_processing));
		UnbindFrameBuffer();

		FrameBuffer shadow_map;
		shadow_map.colour0_texture_attachment.config.texture_format = GL_RG32F;
		shadow_map.colour0_texture_attachment.config.pixel_format = GL_RG;
		shadow_map.colour0_texture_attachment.config.data_type = GL_FLOAT;
		shadow_map.colour0_texture_attachment.config.min_filter = GL_LINEAR;
		shadow_map.colour0_texture_attachment.config.mag_filter = GL_LINEAR;
		shadow_map.colour0_texture_attachment.config.wrap_s_mode = GL_CLAMP_TO_BORDER;
		shadow_map.colour0_texture_attachment.config.wrap_t_mode = GL_CLAMP_TO_BORDER;
		shadow_map.colour0_texture_attachment.config.width = 1024;
		shadow_map.colour0_texture_attachment.config.height = 1024;
		
		CreateTexture(&shadow_map.colour0_texture_attachment, nullptr);
		TextureSetBorder(&shadow_map.colour0_texture_attachment, Vec4(1).arr);

		CreateFrameBuffer(&shadow_map);
		FrameBufferBindColourAttachtments(&shadow_map);
		Assert(CheckFrameBuffer(shadow_map));
		UnbindFrameBuffer();

		FrameBuffer g_buffer;

		g_buffer.colour0_texture_attachment.config.height = WINDOW_HEIGHT;
		g_buffer.colour0_texture_attachment.config.width = WINDOW_WIDTH;
		g_buffer.colour0_texture_attachment.config.data_type = GL_FLOAT;
		g_buffer.colour0_texture_attachment.config.texture_format = GL_RGBA16F;
		g_buffer.colour0_texture_attachment.config.pixel_format = GL_RGBA;
		g_buffer.colour0_texture_attachment.config.wrap_s_mode = GL_CLAMP_TO_EDGE;
		g_buffer.colour0_texture_attachment.config.wrap_t_mode = GL_CLAMP_TO_EDGE;
		g_buffer.colour0_texture_attachment.config.wrap_r_mode = GL_CLAMP_TO_EDGE;

		g_buffer.colour1_texture_attachment.config.height = WINDOW_HEIGHT;
		g_buffer.colour1_texture_attachment.config.width = WINDOW_WIDTH;
		g_buffer.colour1_texture_attachment.config.data_type = GL_FLOAT;
		g_buffer.colour1_texture_attachment.config.texture_format = GL_RGBA16F;
		g_buffer.colour1_texture_attachment.config.pixel_format = GL_RGBA;
		g_buffer.colour1_texture_attachment.config.wrap_s_mode = GL_CLAMP_TO_EDGE;
		g_buffer.colour1_texture_attachment.config.wrap_t_mode = GL_CLAMP_TO_EDGE;
		g_buffer.colour1_texture_attachment.config.wrap_r_mode = GL_CLAMP_TO_EDGE;

		g_buffer.colour2_texture_attachment.config.height = WINDOW_HEIGHT;
		g_buffer.colour2_texture_attachment.config.width = WINDOW_WIDTH;
		g_buffer.colour2_texture_attachment.config.data_type = GL_UNSIGNED_BYTE;
		g_buffer.colour2_texture_attachment.config.texture_format = GL_RGBA;
		g_buffer.colour2_texture_attachment.config.pixel_format   = GL_RGBA;
		g_buffer.colour2_texture_attachment.config.wrap_s_mode = GL_CLAMP_TO_EDGE;
		g_buffer.colour2_texture_attachment.config.wrap_t_mode = GL_CLAMP_TO_EDGE;
		g_buffer.colour2_texture_attachment.config.wrap_r_mode = GL_CLAMP_TO_EDGE;

		g_buffer.render_attchment.width = WINDOW_WIDTH;
		g_buffer.render_attchment.height = WINDOW_HEIGHT;

		CreateTexture(&g_buffer.colour0_texture_attachment, nullptr);
		CreateTexture(&g_buffer.colour1_texture_attachment, nullptr);
		CreateTexture(&g_buffer.colour2_texture_attachment, nullptr);
		CreateFrameBuffer(&g_buffer);

		FrameBufferBindColourAttachtments(&g_buffer);
		FrameAddBufferRenderAttachtment(&g_buffer);
		Assert(CheckFrameBuffer(g_buffer));
		UnbindFrameBuffer();

		FrameBuffer ssao_unblured;
		
		ssao_unblured.colour0_texture_attachment.config.height = WINDOW_HEIGHT;
		ssao_unblured.colour0_texture_attachment.config.width = WINDOW_WIDTH;
		ssao_unblured.colour0_texture_attachment.config.texture_format = GL_R16;
		ssao_unblured.colour0_texture_attachment.config.pixel_format = GL_RED;
		ssao_unblured.colour0_texture_attachment.config.data_type = GL_FLOAT;
		ssao_unblured.colour0_texture_attachment.config.min_filter = GL_NEAREST;
		ssao_unblured.colour0_texture_attachment.config.mag_filter = GL_NEAREST;
		ssao_unblured.colour0_texture_attachment.config.wrap_s_mode = GL_CLAMP_TO_EDGE;
		ssao_unblured.colour0_texture_attachment.config.wrap_t_mode = GL_CLAMP_TO_EDGE;
		ssao_unblured.colour0_texture_attachment.config.wrap_r_mode = GL_CLAMP_TO_EDGE;
	
		CreateTexture(&ssao_unblured.colour0_texture_attachment, nullptr);
		CreateFrameBuffer(&ssao_unblured);

		FrameBufferBindColourAttachtments(&ssao_unblured);		
		Assert(CheckFrameBuffer(ssao_unblured));
		UnbindFrameBuffer();

		FrameBuffer ssao_blured;
		
		ssao_blured.colour0_texture_attachment.config.height = WINDOW_HEIGHT;
		ssao_blured.colour0_texture_attachment.config.width = WINDOW_WIDTH;
		ssao_blured.colour0_texture_attachment.config.texture_format = GL_R16;
		ssao_blured.colour0_texture_attachment.config.pixel_format = GL_RED;
		ssao_blured.colour0_texture_attachment.config.data_type = GL_FLOAT;
		ssao_blured.colour0_texture_attachment.config.min_filter = GL_NEAREST;
		ssao_blured.colour0_texture_attachment.config.mag_filter = GL_NEAREST;
		ssao_blured.colour0_texture_attachment.config.wrap_s_mode = GL_CLAMP_TO_EDGE;
		ssao_blured.colour0_texture_attachment.config.wrap_t_mode = GL_CLAMP_TO_EDGE;
		ssao_blured.colour0_texture_attachment.config.wrap_r_mode = GL_CLAMP_TO_EDGE;

		CreateTexture(&ssao_blured.colour0_texture_attachment, nullptr);
		CreateFrameBuffer(&ssao_blured);

		FrameBufferBindColourAttachtments(&ssao_blured);
		Assert(CheckFrameBuffer(ssao_blured));
		UnbindFrameBuffer();

		frame_post_processing = post_processing;
		frame_shadow_map = shadow_map;
		frame_g_buffer = g_buffer;
		frame_ssao_unblured = ssao_unblured;		
		frame_ssao_blured = ssao_blured;
	}

	void WorldRenderer::InitIdentityTexture()
	{
		Texture id_texture;
		id_texture.config.data_type = GL_UNSIGNED_BYTE;
		id_texture.config.texture_format = GL_RGBA;
		id_texture.config.pixel_format = GL_RGBA;
		id_texture.config.wrap_s_mode = GL_REPEAT;
		id_texture.config.wrap_t_mode = GL_REPEAT;
		id_texture.config.wrap_r_mode = GL_REPEAT;
		id_texture.config.width = 1;
		id_texture.config.height = 1;

		uint8 data[4] = { 255, 255, 255, 255 };

		CreateTexture(&id_texture, data);

		identity_texture = id_texture;
	}

	void WorldRenderer::InitStandardMeshes()
	{
		Assert(0);
	}

	void WorldRenderer::InitSkyBox()
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

	Informer::Informer()
	{

	}

	Informer::~Informer()
	{

	}

	void Informer::CreateUBO(const std::string &name, const LayoutBuffer &lbo, int32 binding_location)
	{
		UniformBuffer ubo;
		ubo.lbo = lbo;
		CreateUniformBuffer(&ubo);
		UniformBufferSetBindPoint(ubo, binding_location);

		ubo_data[name] = binding_location;
		ubos.push_back(ubo);
	}

	void Informer::LinkShader(const std::string &name, Shader shader)
	{
		int32 binding_location = ubo_data[name];
		ShaderBindUniformBuffer(shader, binding_location, name);
	}

}