#pragma once
#include "OpenGL/OpenGl.h"
#include "OpenGL/RenderCommands.h"
#include "Utility.h"
#include "Core/Camera.h"
#include "Core/World.h"
#include "Engine/Input.h"
#include "Engine/AssetLoader.h"
namespace cm
{
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
	
	class Informer
	{
	public:		
		std::unordered_map<std::string, int32> ubo_data;
		std::vector<UniformBuffer> ubos;
	public:
		Informer();
		~Informer();
		
		void CreateUBO(const std::string &name, const LayoutBuffer &lbo, int32 binding_location);
		void LinkShader(const std::string &name, Shader shader);
		void LinkShader(const std::string &name, RenderShaders render_shaders);
		template<typename T>
		void UpdateUBO(const std::string &name, std::vector<T> data) 
		{ WriteBufferData(&ubos[ubo_data.at(name)], data, 0); }

	};

}