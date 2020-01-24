#include "OpenGlRenderer.h"



namespace cm
{
	void CreateBatch(Batch *batch, const VertexBuffer &vbo_to_batch, const IndexBuffer &ibo_of_vbo)
	{
		Assert(batch->vao.object == 0);
		Assert(batch->transforms.size() != 0);
		Assert(batch->copied_ibo.object == 0);
		Assert(ibo_of_vbo.object != 0);
		Assert(vbo_to_batch.object != 0);

		//TODO: This is slow because we read and write. Rather we just copy from one to the other
		uint32 mat_count = (uint32)batch->transforms.size();
		VertexBuffer mat_vbo;
		mat_vbo.lbo = BufferLayout({ ShaderDataType::Mat4 });
		mat_vbo.lbo.SetAttributeDivisor(1);
		CreateBuffer<VertexBuffer>(&mat_vbo, mat_count * sizeof(Mat4), VertexFlags::READ_WRITE);
		WriteBufferData(mat_vbo, batch->transforms, 0);

		VertexBuffer current_mesh_vbo = vbo_to_batch;
		DynaArray<float> current_mesh_data;
		ReadBufferData(current_mesh_vbo, &current_mesh_data, current_mesh_vbo.size_bytes, 0);

		VertexBuffer mesh_vbo;
		mesh_vbo.lbo = current_mesh_vbo.lbo;
		CreateBuffer<VertexBuffer>(&mesh_vbo, current_mesh_vbo.size_bytes, VertexFlags::READ_WRITE);
		WriteBufferData(mesh_vbo, current_mesh_data, 0);

		IndexBuffer ibo;
		CreateBuffer<IndexBuffer>(&ibo, ibo_of_vbo.size_bytes, VertexFlags::READ_WRITE);
		ibo.index_count = ibo_of_vbo.index_count;

		DynaArray<uint32> index_data;
		ReadBufferData(ibo_of_vbo, &index_data, ibo_of_vbo.size_bytes, 0);

		WriteBufferData(ibo, index_data, 0);

		VertexArray temp_vao;
		temp_vao.vertex_buffers.push_back(mesh_vbo);
		temp_vao.vertex_buffers.push_back(mat_vbo);

		CreateVertexArray(&temp_vao);


		batch->vao = temp_vao;
		batch->copied_ibo = ibo;
	}

	void FreeBatch(Batch *batch)
	{
		//TODO: Free all the batch memory;
	}


	void RenderBatch(const Shader &shader, const Batch &batch)
	{
		uint32 amount = (uint32)batch.transforms.size();
		//@NOTE: Test to make sure we haven't added anything to the transforms without recreating batch
		Assert(batch.vao.vertex_buffers[1].size_bytes / sizeof(Mat4) == amount);


		BindShader(shader);
		BindVertexArray(batch.vao);
		BindBuffer<IndexBuffer>(batch.copied_ibo);

		glDrawElementsInstanced(GL_TRIANGLES, batch.copied_ibo.index_count,
			GL_UNSIGNED_INT, 0, amount);

		UnbindBuffer<IndexBuffer>(batch.copied_ibo);
		UnbindVertexArray();
	}

	void RenderMesh(const Shader &shader, const Mesh &mesh)
	{
		BindShader(shader);

		BindVertexArray(mesh.vao);
		BindBuffer<IndexBuffer>(mesh.ibo);

		glDrawElements(GL_TRIANGLES, mesh.ibo.index_count, GL_UNSIGNED_INT, 0);

		UnbindBuffer<IndexBuffer>(mesh.ibo);
		UnbindVertexArray();
	}

	void RenderWorld(Shader *shader, const World &world)
	{

	}
}