#include "Sandbox.h"
namespace cm
{
	cm::GLMesh *Actor::GetMeshForRender()
	{
		return &mesh;
	}	   	  
	   
	cm::Material *Actor::GetMaterial() 
	{
		return &material;
	}

	cm::Transform *Actor::GetTransform()
	{
		return &transform;
	}

	cm::GeometricCollider *Actor::GetCollider()
	{	
		if (obb.extents == Vec3(0))
		{
			return nullptr;
		}
		return &obb;
	}

	const cm::Mat4 Actor::GetTransformMatrix() const
	{
		return transform.CalcTransformMatrix();
	}

	const uint32 Actor::GetRenderFlags() const
	{
		return 0;
	}

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

	cm::Actor CreateActorCube()
	{
		Assert(StandardMeshes::IsInitilized());

		Actor cube;
		
		cube.material.diffuse = Vec3(0.85);
		cube.material.roughness = 0.8;
		cube.material.metalness = 0.1;

		cube.obb = OBB(Vec3(0), Vec3(1), Basis());

		cube.mesh = StandardMeshes::Cube();

		return cube;
	}

}