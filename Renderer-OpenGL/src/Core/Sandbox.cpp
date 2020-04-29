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

	const cm::Mat4 AnimatedActor::GetTransformMatrix() const
	{
		return transform.CalcTransformMatrix();
	}

	const uint32 AnimatedActor::GetRenderFlags() const
	{
		return 0;
	}
}