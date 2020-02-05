#include "Mesh.h"

namespace cm
{

	Mesh EditableMesh::CreateMesh()
	{
		DynaArray<float> vert_data;
		DynaArray<uint32> index_data;
		RecaluclateNormals();
		ConvertToPNT(&vert_data, &index_data);


		VertexBuffer vbo;
		vbo.lbo = PNT_VBO_LAYOUT;



		CreateBuffer<VertexBuffer>(&vbo, vert_data.size() * sizeof(float), VertexFlags::READ_WRITE);
		WriteBufferData(vbo, vert_data, 0);

		IndexBuffer ibo;
		ibo.index_count = (uint32)index_data.size();
		CreateBuffer<IndexBuffer>(&ibo, index_data.size() * sizeof(uint32), VertexFlags::READ_WRITE);
		WriteBufferData(ibo, index_data, 0);

		VertexArray vao;
		vao.vertex_buffers.push_back(vbo);
		CreateVertexArray(&vao);

		Mesh mesh;
		mesh.vao = vao;
		mesh.ibo = ibo;
		return mesh;
	}

	void EditableMesh::AddTrianlge(const Vec3 &pos1, const Vec3 &pos2, const Vec3 &pos3)
	{
		IndexedTriangle tri;
		Vertex p1;
		p1.position = pos1;
		p1.texture_coord = pos1;
		vertices.push_back(p1);
		tri.v1 = static_cast<uint32>(vertices.size()) - 1;

		Vertex p2;
		p2.position = pos2;
		p2.texture_coord = pos2;
		vertices.push_back(p2);
		tri.v2 = static_cast<uint32>(vertices.size()) - 1;

		Vertex p3;
		p3.position = pos3;
		p3.texture_coord = pos3;
		vertices.push_back(p3);
		tri.v3 = static_cast<uint32>(vertices.size()) - 1;

		tri.edge1 = pos2 - pos1;
		tri.edge2 = pos3 - pos2;
		tri.edge3 = pos1 - pos3;

		tri.CalculateNormal();
		tris.push_back(tri);
	}

	void EditableMesh::AddTrianlge(const Vertex &p1, const Vertex &p2, const Vertex &p3)
	{
		vertices.push_back(p1);
		vertices.push_back(p2);
		vertices.push_back(p3);
		uint32 count_index = static_cast<uint32>(vertices.size()) - 1;
		IndexedTriangle tri;

		tri.v1 = count_index - 2;
		tri.v2 = count_index - 1;
		tri.v3 = count_index;

		tri.edge1 = p2.position - p1.position;
		tri.edge2 = p3.position - p2.position;
		tri.edge3 = p1.position - p3.position;

		tri.CalculateNormal();
		tris.push_back(tri);
	}

	void EditableMesh::AddTextureCoords(const Vec3 &t1, const Vec3 &t2, const Vec3 &t3)
	{
		uint32 size = static_cast<uint32>(vertices.size());
		vertices[size - 1].texture_coord = t3;
		vertices[size - 2].texture_coord = t2;
		vertices[size - 3].texture_coord = t1;
	}

	void EditableMesh::RecaluclateNormals()
	{
		uint32 size = static_cast<uint32>(tris.size());
		for (uint32 i = 0; i < size; i++)
		{
			tris[i].CalculateNormal();
		}
	}

	void EditableMesh::FuseVertices(float tollerance)
	{
		//https://answers.unity.com/questions/228841/dynamically-combine-verticies-that-share-the-same.html
		uint32 size = static_cast<uint32>(vertices.size());
		uint32 half_size = (uint32)std::ceilf((float)size / 2.f);

		DynaArray<uint32> marked_doubles;
		marked_doubles.resize(vertices.size());

		for (uint32 i = 0; i < half_size; i++)
		{
			Vertex cur = vertices[i];
			for (uint32 ii = i + 1; ii < size; ii++)
			{
				Vertex temp = vertices[ii];
				Vec3 a1 = cur.position;
				Vec3 b1 = temp.position;

				Vec3 n1 = cur.normal;
				Vec3 n2 = temp.normal;
				bool res = CompareVec(a1, b1, tollerance) && CompareVec(n1, n2, tollerance);
				if (res)
				{
					marked_doubles[ii] = i;
					
				}
			}
		}

		uint32 tri_size = static_cast<uint32>(tris.size());
		uint32 merge_count = 0;
		uint32 remained_count = 0;
		//@TODO: We don't care about the first tri. We could more optimized by skipping it in the marked_double.
		//@TODO: We can't merge the vert first index cause we treat 0 as not being flagged.
		for (uint32 i = 1; i < tri_size; i++)
		{
			IndexedTriangle tri = tris[i];
			bool res1 = (bool)marked_doubles[tri.v1];
			bool res2 = (bool)marked_doubles[tri.v2];
			bool res3 = (bool)marked_doubles[tri.v3];
			bool res[3] = { res1, res2, res3 };

			uint32 rem = 0;
			for (int32 ii = 0; ii < 3; ii++)
			{
				if (res[ii])
				{
					tris[i].index[ii] = marked_doubles[tri.index[ii]] - remained_count;
					merge_count++;
				}
				else
				{
					tris[i].index[ii] -= merge_count;
					rem++;
				}
			}
			remained_count += rem;
		}
		merge_count = 0;
		for (uint32 i = 0; i < marked_doubles.size(); i++)
		{
			if (marked_doubles[i])
			{
				//@NOTE: Leave these brackets, some pointer arithmetic nonsense
				auto index = vertices.begin() + (i - merge_count); 
				merge_count++;
				
				vertices.erase(index);
			}
		}


	}


	void EditableMesh::ConvertToPNT(DynaArray<float> *data, DynaArray<uint32> *index) const
	{
		//@Speed: Prehaps resize arr before hand
		uint32 index_counter = 0;

		uint32 vert_size = static_cast<uint32>(vertices.size());
		for (uint32 i = 0; i < vert_size; i++)
		{
			Vertex vert = vertices[i];
			data->CopyFromPtr(vert.position.arr, 3 * sizeof(float));
			data->CopyFromPtr(vert.normal.arr, 3 * sizeof(float));
			data->push_back(vert.texture_coord.x);
			data->push_back(vert.texture_coord.y);
		}


		uint32 index_size = static_cast<uint32>(tris.size());
		for (uint32 i = 0; i < index_size; i++)
		{
			IndexedTriangle tri = tris[i];
			index->push_back(tri.v1);
			index->push_back(tri.v2);
			index->push_back(tri.v3);
		}
		int a = 2;
	}

	void EditableMesh::ConvertToOther(DynaArray<float> *data, DynaArray<uint32> *index) const
	{
		uint32 vert_size = static_cast<uint32>(vertices.size());
		for (uint32 i = 0; i < vert_size; i++)
		{
			Vertex vert = vertices[i];
			data->CopyFromPtr(vert.position.arr, 3 * sizeof(float));
			data->CopyFromPtr(vert.normal.arr, 3 * sizeof(float));
			data->push_back(vert.colour.x);
			data->push_back(vert.colour.y);
			data->push_back(vert.colour.z);
		}


		uint32 index_size = static_cast<uint32>(tris.size());
		for (uint32 i = 0; i < index_size; i++)
		{
			IndexedTriangle tri = tris[i];
			index->push_back(tri.v1);
			index->push_back(tri.v2);
			index->push_back(tri.v3);
		}
		int a = 2;
	}


	void EditableMesh::SetColour(const Vec3 colour)
	{
		vertices[vertices.size() - 1].colour = colour;
		vertices[vertices.size() - 2].colour = colour;
		vertices[vertices.size() - 3].colour = colour;
	}

}
