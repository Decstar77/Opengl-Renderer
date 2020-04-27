#include "EditableMesh.h"

namespace cm
{

	GLMesh EditableMesh::CreateMesh(bool tangets)
	{
		// @TODO: Make function faster
		// @NOTE: So, we have all out data store is aos and we have to convert it to a soa. For two reasons.
		//		1) The Vec3 class is padded with an extra 4 bytes, float
		//		2) We might not use all the data stored in the vertex, ie vert colours, tangets.


		std::vector<real> vert_data;
		std::vector<uint32> index_data;
		LayoutBuffer layout = {};
		if (tangets)
		{
			layout = PNTTB_VBO_LAYOUT;
			ConvertToOther(&vert_data, &index_data);
			index_data = indices;
		}
		else
		{
			if (indices.size() == 0)
			{
				ConvertToPNT(&vert_data, &index_data);
			}
			else
			{
				ConvertToPNT(&vert_data, nullptr);
				index_data = indices; // @SPEED: SLOW!!!!!!!!!!!!! COPY!!!!!!
			}

			layout = PNT_VBO_LAYOUT;
		}

		VertexBuffer vbo;
		vbo.lbo = layout;
		vbo.size_bytes = static_cast<uint32>(vert_data.size() * sizeof(real));
		vbo.flags = VertexFlags::READ_WRITE; // Optional;

		CreateVertexBuffer(&vbo);
		WriteBufferData(&vbo, vert_data, 0);

		IndexBuffer ibo;
		ibo.index_count = (uint32)index_data.size();
		ibo.size_bytes = static_cast<uint32>( index_data.size() * sizeof(uint32));
		ibo.flags = VertexFlags::READ_WRITE;
		
		CreateIndexBuffer(&ibo);
		WriteBufferData(&ibo, index_data, 0);

		VertexArray vao;
		vao.vertex_buffers.push_back(vbo);
		CreateVertexArray(&vao);

		GLMesh mesh;
		mesh.vao = vao;
		mesh.ibo = ibo;
		return mesh;
	}

	cm::GLMesh EditableMesh::CreateMesh()
	{
		GLMesh mesh;

		//LayoutBuffer l = LayoutBuffer(std::vector<ShaderDataType>({ ShaderDataType::Float3, ShaderDataType::Float3, ShaderDataType::Float2, ShaderDataType::Float4, ShaderDataType::Float4 }));
		LayoutBuffer l = LayoutBuffer(std::vector<ShaderDataType>({
			ShaderDataType::Float3, //vpos
			ShaderDataType::Float3, //vnorm
			ShaderDataType::Float2,	//vtext
			}));
		std::vector<real32> data;
		uint32 vert_size = static_cast<uint32>(vertices.size());
		for (uint32 i = 0; i < vert_size; i++)
		{
			Vertex vert = vertices[i];
			data.push_back(vert.position.x);
			data.push_back(vert.position.y);
			data.push_back(vert.position.z);

			data.push_back(vert.normal.x);
			data.push_back(vert.normal.y);
			data.push_back(vert.normal.z);

			data.push_back(vert.texture_coord.x);
			data.push_back(vert.texture_coord.y);
		}
		
		CreateGLMesh(&mesh, l, data.data(), data.size() * sizeof(real32), indices.data(), indices.size() * sizeof(uint32));
		return mesh;
	}

	void EditableMesh::ConvertToPNT(std::vector<float> *data, std::vector<uint32> *index) const
	{
		//@Speed: Prehaps resize arr before hand
		uint32 index_counter = 0;

		uint32 vert_size = static_cast<uint32>(vertices.size());
		for (uint32 i = 0; i < vert_size; i++)
		{
			Vertex vert = vertices[i];
			data->insert(data->end(), &vert.position.arr[0], &vert.position.arr[3 * sizeof(float) / sizeof(float)]);
			data->insert(data->end(), &vert.normal.arr[0], &vert.normal.arr[3 * sizeof(float) / sizeof(float)]);
			data->push_back(vert.texture_coord.x);
			data->push_back(vert.texture_coord.y);
		}
		if (index != nullptr)
		{

		}
		
	}

	void EditableMesh::ConvertToOther(std::vector<float> *data, std::vector<uint32> *index) const
	{
		uint32 vert_size = static_cast<uint32>(vertices.size());
		for (uint32 i = 0; i < vert_size; i++)
		{
			Vertex vert = vertices[i];
			data->insert(data->end(), &vert.position.arr[0], &vert.position.arr[3 * sizeof(float) / sizeof(float)]);
			data->insert(data->end(), &vert.normal.arr[0], &vert.normal.arr[3 * sizeof(float) / sizeof(float)]);
			data->push_back(vert.texture_coord.x);
			data->push_back(vert.texture_coord.y);
			data->insert(data->end(), &vert.tangent.arr[0], &vert.tangent.arr[3 * sizeof(float) / sizeof(float)]);
			data->insert(data->end(), &vert.bitangent.arr[0], &vert.bitangent.arr[3 * sizeof(float) / sizeof(float)]);

		}
	}

	cm::GLMesh EditableMesh::CreateAnimMesh()
	{
		//LayoutBuffer l = LayoutBuffer(std::vector<ShaderDataType>({ ShaderDataType::Float3, ShaderDataType::Float3, ShaderDataType::Float2, ShaderDataType::Float4, ShaderDataType::Float4 }));
		LayoutBuffer l = LayoutBuffer(std::vector<ShaderDataType>({ 
			ShaderDataType::Float3, //vpos
			ShaderDataType::Float3, //vnorm
			ShaderDataType::Float2,	//vtext
			ShaderDataType::Float3,	//tan
			ShaderDataType::Float3,	//bitan
			ShaderDataType::Float4, //bone index
			ShaderDataType::Float4  //bone weight
			}));
		std::vector<real32> data;
		uint32 vert_size = static_cast<uint32>(vertices.size());
		for (uint32 i = 0; i < vert_size; i++)
		{
			Vertex vert = vertices[i];
			data.push_back(vert.position.x);
			data.push_back(vert.position.y);
			data.push_back(vert.position.z);

			data.push_back(vert.normal.x);
			data.push_back(vert.normal.y);
			data.push_back(vert.normal.z);

			data.push_back(vert.texture_coord.x);
			data.push_back(vert.texture_coord.y);
								
			data.push_back(vert.tangent.x);
			data.push_back(vert.tangent.y);
			data.push_back(vert.tangent.z);

			data.push_back(vert.bitangent.x);
			data.push_back(vert.bitangent.y);
			data.push_back(vert.bitangent.z);

			data.push_back(static_cast<real32>(vert.bone_index[0]));
			data.push_back(static_cast<real32>(vert.bone_index[1]));
			data.push_back(static_cast<real32>(vert.bone_index[2]));
			data.push_back(static_cast<real32>(vert.bone_index[3]));

			data.push_back(vert.bone_weights[0]);
			data.push_back(vert.bone_weights[1]);
			data.push_back(vert.bone_weights[2]);
			data.push_back(vert.bone_weights[3]);
		}

		VertexBuffer vbo;
		vbo.lbo = l;
		vbo.size_bytes = static_cast<uint32>(data.size() * sizeof(real));
		vbo.flags = VertexFlags::READ_WRITE; // Optional;

		CreateVertexBuffer(&vbo);
		WriteBufferData(&vbo, data, 0);

		IndexBuffer ibo;
		ibo.index_count = (uint32)indices.size();
		ibo.size_bytes = static_cast<uint32>(indices.size() * sizeof(uint32));
		ibo.flags = VertexFlags::READ_WRITE;

		CreateIndexBuffer(&ibo);
		WriteBufferData(&ibo, indices, 0);

		VertexArray vao;
		vao.vertex_buffers.push_back(vbo);
		CreateVertexArray(&vao);

		GLMesh mesh;
		mesh.vao = vao;
		mesh.ibo = ibo;
		return mesh;
	}

	
}
