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
				Mat4 transform = a->GetTransformMatrix();

				ShaderSetMat4(shader, "model", transform.arr);
				RenderMesh(*shader, mesh);
			}
		}
	}


}
