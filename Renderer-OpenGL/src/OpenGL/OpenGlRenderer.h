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
	
	void RenderMesh(Shader *shader, const Mesh &mesh)
	{
		BindShader(*shader);

		BindVertexArray(mesh.vao);
		BindBuffer<IndexBuffer>(mesh.ibo);

		glDrawElements(GL_TRIANGLES, mesh.ibo.index_count, GL_UNSIGNED_INT, 0);

		UnbindBuffer<IndexBuffer>(mesh.ibo);
		UnbindVertexArray();
	}
	
	FrameBuffer EquirectangularToCubemap(const Texture &hdr_texture, CubeMap *enviroment_map, CubeMap *irradiance_map,
		Shader *equirectangular_to_cubemap_shader, Shader *irradiance_shader, Mesh cube_mesh)
	{
		FrameBuffer fbo;
		fbo.config.render_buffer_format = GL_DEPTH_COMPONENT24;
		fbo.config.render_buffer_attachment = GL_DEPTH_ATTACHMENT;		
		CreateFrameBuffer(&fbo);
		AddFrameBufferRenderAttachtment(&fbo, 512, 512);
			   
		CubeMap env;	
		env.config.type = GL_TEXTURE_CUBE_MAP;
		env.config.texture_format = GL_RGB16F;
		env.config.pixel_format = GL_RGB;
		env.config.data_type = GL_FLOAT;
		env.config.wrap_s_mode = GL_CLAMP_TO_EDGE;
		env.config.wrap_t_mode = GL_CLAMP_TO_EDGE;
		env.config.wrap_r_mode = GL_CLAMP_TO_EDGE;
		env.config.mag_filter = GL_LINEAR;
		env.config.min_filter = GL_LINEAR;
		env.config.width = 512;
		env.config.height = 512;				
		CreateCubeMap<void *>(&env, nullptr);

		Mat4 captureProjection = perspective(90.0f, 1.0f, 0.1f, 10.0f);
		Mat4 captureViews[] =
		{
			LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f,  0.0f,  0.0f), Vec3(0.0f, -1.0f,  0.0f)),
			LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(-1.0f,  0.0f,  0.0f),Vec3(0.0f, -1.0f,  0.0f)),
			LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f,  1.0f,  0.0f), Vec3(0.0f,  0.0f,  1.0f)),
			LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, -1.0f,  0.0f), Vec3(0.0f,  0.0f, -1.0f)),
			LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f,  0.0f,  1.0f), Vec3(0.0f, -1.0f,  0.0f)),
			LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f,  0.0f, -1.0f), Vec3(0.0f, -1.0f,  0.0f))
		};

		BindShader(*equirectangular_to_cubemap_shader);
		ShaderSetInt32(*equirectangular_to_cubemap_shader, "remove_translation", false);
		ShaderSetInt32(*equirectangular_to_cubemap_shader, "equirectangularMap", 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, hdr_texture.object);

		glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
		glBindFramebuffer(GL_FRAMEBUFFER, fbo.object);
		ShaderSetMat4(*equirectangular_to_cubemap_shader, "projection", captureProjection.arr);

		for (unsigned int i = 0; i < 6; ++i)
		{
			ShaderSetMat4(*equirectangular_to_cubemap_shader, "view", captureViews[i].arr);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
				env.object, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			RenderMesh(equirectangular_to_cubemap_shader, cube_mesh);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		CubeMap irr;
		irr.config.type = GL_TEXTURE_CUBE_MAP;
		irr.config.texture_format = GL_RGB16F;
		irr.config.pixel_format = GL_RGB;
		irr.config.data_type = GL_FLOAT;
		irr.config.wrap_s_mode = GL_CLAMP_TO_EDGE;
		irr.config.wrap_t_mode = GL_CLAMP_TO_EDGE;
		irr.config.wrap_r_mode = GL_CLAMP_TO_EDGE;
		irr.config.mag_filter = GL_LINEAR;
		irr.config.min_filter = GL_LINEAR;
		irr.config.width = 512;
		irr.config.height = 512;
		CreateCubeMap<void *>(&irr, nullptr);


		glBindFramebuffer(GL_FRAMEBUFFER, fbo.object);
		glBindRenderbuffer(GL_RENDERBUFFER, fbo.render_object);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

		BindShader(*irradiance_shader);
		ShaderSetInt32(*irradiance_shader, "environmentMap", 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, env.object);

		glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
		glBindFramebuffer(GL_FRAMEBUFFER, fbo.object);
		ShaderSetMat4(*irradiance_shader, "projection", captureProjection.arr);
		for (unsigned int i = 0; i < 6; ++i)
		{
			ShaderSetMat4(*irradiance_shader, "view", captureViews[i].arr);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irr.object, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			RenderMesh(irradiance_shader, cube_mesh);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, 1280, 780);	//@Hack		   
		
		//Free memory and shaders
		glDeleteFramebuffers(1, &fbo.object);

		*enviroment_map = env;
		*irradiance_map = irr;


		return fbo;
	}

	void RenderWorld(World *world)
	{
		/*
		Forward Renderer
		{
			Set lighting UBO info
			Set Camera UBO info
			Do batching, we can use the Material Shader ID.
		
			Shadow Pass
		
		
			Render Pass For each batch/draw
			{
				Bind VAO
				Set textures
				Do drawing
				UNBIND THINGS
			}
		}

		*/
	}

	


}

