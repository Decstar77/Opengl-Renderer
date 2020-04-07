#include "World.h"

namespace cm
{

	void RenderWorld(Shader *shader, Shader *batch_shader, const World &world)
	{

		//if (batch_shader != nullptr)
		//{
		//	for (int32 i = 0; i < world.batches.size(); i++)
		//	{
		//		RenderBatch(*batch_shader, world.batches[i]);
		//	}
		//}

		if (shader != nullptr)
		{
			BindShader(*shader);
			for (int32 i = 0; i < world.objects.size(); i++)
			{
				WorldObject* a = world.objects[i];
				GLMesh mesh = a->GetMeshForRender();

				a->SetTransformValues(shader);				
				RenderMesh(*shader, mesh);
			}
		}
	}

	void Actor::SetTextures(Shader *shader)
	{
		if (material.diffuse_texture)
		{
			ShaderBindTexture(*shader, *material.diffuse_texture, 0, "colour_map");
		}

		if (material.normal_texture)
		{
			ShaderBindTexture(*shader, *material.normal_texture, 1, "normal_map");
		}

		if (material.occlusion_roughness_metallic)
		{
			ShaderBindTexture(*shader, *material.occlusion_roughness_metallic, 2, "oc_r_m_map");
		}

		if (material.emssive_texture)
		{
			ShaderBindTexture(*shader, *material.occlusion_roughness_metallic, 3, "emssive_map");
		}

	}

	void Actor::SetMaterialValues(Shader *shader)
	{
		ShaderSetVec3(shader, "diffuse_colour", Vec3(0.23, 0.48, 0.34).arr);

		ShaderSetVec3(shader, "specular_colour", Vec3(0.2f).arr);
	}

	void Actor::SetTransformValues(Shader *shader)
	{
		ShaderSetMat4(shader, "model", transform.CalcTransformMatrix().arr);
	}

	const cm::GLMesh & Actor::GetMeshForRender()
	{
		return mesh;
	}
   	  
}
