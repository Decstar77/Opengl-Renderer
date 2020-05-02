#include "Sandbox.h"
namespace cm
{
	cm::Material *AnimatedActor::GetMaterial() 
	{
		return &material;
	}

	cm::Transform *AnimatedActor::GetTransform()
	{
		return &transform;
	}

	cm::GLMesh *AnimatedActor::GetMeshForRender()
	{
		return &mesh;
	}

	cm::GeometricCollider * AnimatedActor::GetCollider()
	{
		return nullptr;
	}

	uint32 AnimatedActor::GetRenderFlags() const
	{
		return 0;
	}
}