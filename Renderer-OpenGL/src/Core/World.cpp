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
		if (obb.GetExtents() == Vec3(0))
		{
			return nullptr;
		}
		return &obb;
	}

	uint32 StaticWorldObject::GetRenderFlags() const
	{
		return render_flags;
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

	bool Material::HasTextures() const
	{
		return (diffuse_texture != nullptr) || (normal_texture != nullptr) || (occlusion_roughness_metallic != nullptr);
	}

	void Material::SetTextures(Shader *shader)
	{
		Assert(diffuse_texture != nullptr);
		Assert(normal_texture!= nullptr);
		Assert(occlusion_roughness_metallic != nullptr);
		// @TODO: Texture storage look-up
		ShaderBindTexture(*shader, *diffuse_texture, 0, "colour_map");
		ShaderBindTexture(*shader, *normal_texture, 1, "normal_map");
		ShaderBindTexture(*shader, *occlusion_roughness_metallic, 2, "orme_map");
	}

	void Material::SetValues(Shader *shader)
	{		
		ShaderSetVec3(shader, "colour_set", diffuse.arr);		
		ShaderSetVec3(shader, "orm_set", Vec3(1, roughness, metalness).arr);
	}

}
