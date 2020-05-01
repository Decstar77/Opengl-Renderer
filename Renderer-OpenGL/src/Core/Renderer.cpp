#include "Core/Renderer.h"
namespace cm
{
	Informer::Informer()
	{

	}

	Informer::~Informer()
	{

	}

	void Informer::CreateUBO(const String &name, const LayoutBuffer &lbo, int32 binding_location)
	{
		UniformBuffer ubo;
		ubo.lbo = lbo;
		CreateUniformBuffer(&ubo);
		UniformBufferSetBindPoint(ubo, binding_location);

		ubo_data[name] = binding_location;
		ubos.push_back(ubo);
	}

	void Informer::LinkShader(const String &name, Shader shader)
	{
		int32 binding_location = ubo_data[name];
		ShaderBindUniformBuffer(shader, binding_location, name);
	}

}