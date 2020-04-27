#include "Sandbox.h"



namespace cm
{
//	void Actor::SetTextures(Shader *shader)
//	{
//		if (material.diffuse_texture)
//		{
//			ShaderBindTexture(*shader, *material.diffuse_texture, 0, "colour_map");
//		}
//
//		if (material.normal_texture)
//		{
//			ShaderBindTexture(*shader, *material.normal_texture, 1, "normal_map");
//		}
//
//		if (material.occlusion_roughness_metallic)
//		{
//			ShaderBindTexture(*shader, *material.occlusion_roughness_metallic, 2, "oc_r_m_map");
//		}
//
//		if (material.emssive_texture)
//		{
//			ShaderBindTexture(*shader, *material.occlusion_roughness_metallic, 3, "emssive_map");
//		}
//	}

//	void Actor::SetMaterialValues(Shader *shader)
//	{		
//		ShaderSetFloat(shader, "material_roughness", material.roughness);
//		//ShaderSetVec3(shader, "specular_colour", Vec3(0.2f).arr);
//	}
//
//	void Actor::SetTransformValues(Shader *shader)
//	{
//		ShaderSetMat4(shader, "model", transform.CalcTransformMatrix().arr);
//	}


	//ShaderSetVec3(shader, "diffuse_colour", Vec3(0.23, 0.48, 0.34).arr);
	//ShaderSetVec3(shader, "specular_colour", Vec3(0.2f).arr);

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

	const cm::Mat4 AnimatedActor::GetTransformMatrix() const
	{
		return transform.CalcTransformMatrix();
	}

	const uint32 AnimatedActor::GetRenderFlags() const
	{
		return 0;
	}


}