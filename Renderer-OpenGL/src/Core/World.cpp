#include "World.h"

namespace cm
{
	cm::GLMesh *StaticWorldObject::GetMeshForRender()
	{
		return &mesh;
	}

	cm::Material *StaticWorldObject::GetMaterial()
	{
		return &material;
	}

	cm::Transform *StaticWorldObject::GetTransform()
	{
		return &transform;
	}

	cm::GeometricCollider *StaticWorldObject::GetCollider()
	{
		if (obb.extents == Vec3(0))
		{
			return nullptr;
		}
		return &obb;
	}

	const cm::Mat4 StaticWorldObject::GetTransformMatrix() const
	{
		return transform.CalcTransformMatrix();
	}

	const uint32 StaticWorldObject::GetRenderFlags() const
	{
		return 0;
	}

	void World::RegisterWorldObject(WorldObject *object)
	{
		// @NOTE: This will get more complex later
		Assert(object != nullptr);
		this->objects.push_back(object);
	}

	cm::StaticWorldObject* CreateWorldObjectCube()
	{
		Assert(StandardMeshes::IsInitilized());

		StaticWorldObject *cube = new StaticWorldObject();

		cube->material.diffuse = Vec3(0.85);
		cube->material.roughness = 0.8;
		cube->material.metalness = 0.1;

		cube->obb = OBB(Vec3(0), Vec3(1), Basis());

		cube->mesh = StandardMeshes::Cube();

		return cube;
	}

	cm::StaticWorldObject* CreateWorldObjectPlane()
	{
		Assert(StandardMeshes::IsInitilized());

		StaticWorldObject *plane = new StaticWorldObject();

		plane->material.diffuse = Vec3(0.85);
		plane->material.roughness = 0.8;
		plane->material.metalness = 0.1;

		plane->obb = OBB(Vec3(0), Vec3(1, 1, 0), Basis());

		plane->mesh = StandardMeshes::Plane();

		return plane;
	}

}
