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
	class Informer
	{
	public:		
		std::unordered_map<String, int32> ubo_data;
		std::vector<UniformBuffer> ubos;
	public:
		Informer();
		~Informer();
		
		void CreateUBO(const String &name, const LayoutBuffer &lbo, int32 binding_location);
		void LinkShader(const String &name, Shader shader);
		template<typename T>
		void UpdateUBO(const String &name, std::vector<T> data);

	};

	template<typename T>
	void cm::Informer::UpdateUBO(const String &name, std::vector<T> data)
	{
		WriteBufferData(&ubos[ubo_data.at(name)], data, 0);
	}

}