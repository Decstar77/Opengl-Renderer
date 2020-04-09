#include "EditableMesh.h"

namespace cm
{

	GLMesh EditableMesh::CreateMesh(bool tangets)
	{
		// @TODO: Make function faster
		// @NOTE: So, we have all out data store is aos and we have to convert it to a soa. For two reasons.
		//		1) The Vec3 class is padded with an extra 4 bytes, float
		//		2) We might not use all the data stored in the vertex, ie vert colours, tangets.


		DynaArray<real> vert_data;
		DynaArray<uint32> index_data;
		BufferLayout layout = {};
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
		vbo.size_bytes = vert_data.size() * sizeof(real);
		vbo.flags = VertexFlags::READ_WRITE; // Optional;

		CreateVertexBuffer(&vbo);
		WriteBufferData(&vbo, vert_data, 0);

		IndexBuffer ibo;
		ibo.index_count = (uint32)index_data.size();
		ibo.size_bytes = index_data.size() * sizeof(uint32);
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
			//TODO: Each vertex could have a different normal, in fact it will for a hard shaded cube. Refactor			
			vertices[tris[i].v1].normal = tris[i].normal;
			vertices[tris[i].v2].normal = tris[i].normal;
			vertices[tris[i].v3].normal = tris[i].normal;
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
		if (index != nullptr)
		{
			uint32 index_size = static_cast<uint32>(tris.size());
			for (uint32 i = 0; i < index_size; i++)
			{
				IndexedTriangle tri = tris[i];
				index->push_back(tri.v1);
				index->push_back(tri.v2);
				index->push_back(tri.v3);
			}
		}
		
	}

	void EditableMesh::ConvertToOther(DynaArray<float> *data, DynaArray<uint32> *index) const
	{
		uint32 vert_size = static_cast<uint32>(vertices.size());
		for (uint32 i = 0; i < vert_size; i++)
		{
			Vertex vert = vertices[i];
			data->CopyFromPtr(vert.position.arr, 3 * sizeof(float));
			data->CopyFromPtr(vert.normal.arr, 3 * sizeof(float));
			data->push_back(vert.texture_coord.x);
			data->push_back(vert.texture_coord.y);
			data->CopyFromPtr(vert.tanget.arr, 3 * sizeof(float));
			data->CopyFromPtr(vert.bitanget.arr, 3 * sizeof(float));
		}
	}


	void EditableMesh::SetColour(const Vec3 colour)
	{
		vertices[vertices.size() - 1].colour = colour;
		vertices[vertices.size() - 2].colour = colour;
		vertices[vertices.size() - 3].colour = colour;
	}
	   	  

	cm::GLMesh EditableMesh::CreateAnimMesh()
	{
		BufferLayout l = BufferLayout(DynaArray<ShaderDataType>({ ShaderDataType::Float3, ShaderDataType::Float3, ShaderDataType::Float2, ShaderDataType::Float4, ShaderDataType::Float4 }));

		DynaArray<float> data;
		uint32 vert_size = static_cast<uint32>(vertices.size());
		for (uint32 i = 0; i < vert_size; i++)
		{
			Vertex vert = vertices[i];
			data.CopyFromPtr(vert.position.arr, 3 * sizeof(float));
			data.CopyFromPtr(vert.normal.arr, 3 * sizeof(float));
			data.push_back(vert.texture_coord.x);
			data.push_back(vert.texture_coord.y);

					
			data.push_back(vert.bone_index[0]);
			data.push_back(vert.bone_index[1]);
			data.push_back(vert.bone_index[2]);
			data.push_back(vert.bone_index[3]);
			data.CopyFromPtr(vert.bone_weights, 4 * sizeof(float));

		}

		VertexBuffer vbo;
		vbo.lbo = l;
		vbo.size_bytes = data.size() * sizeof(real);
		vbo.flags = VertexFlags::READ_WRITE; // Optional;

		CreateVertexBuffer(&vbo);
		WriteBufferData(&vbo, data, 0);

		IndexBuffer ibo;
		ibo.index_count = (uint32)indices.size();
		ibo.size_bytes = indices.size() * sizeof(uint32);
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

	GLMesh EditableMesh::CreateAnimMesh(const std::vector<VertexBoneInfo> & vertex_information)
	{
		// @TODO: Clean
		BufferLayout l = BufferLayout(DynaArray<ShaderDataType>({ ShaderDataType::Float3, ShaderDataType::Float3, ShaderDataType::Float2, ShaderDataType::Float4, ShaderDataType::Float4 }));

		DynaArray<float> data;
		uint32 vert_size = static_cast<uint32>(vertices.size());
		for (uint32 i = 0; i < vert_size; i++)
		{
			Vertex vert = vertices[i];
			data.CopyFromPtr(vert.position.arr, 3 * sizeof(float));
			data.CopyFromPtr(vert.normal.arr, 3 * sizeof(float));
			data.push_back(vert.texture_coord.x);
			data.push_back(vert.texture_coord.y);

			// @NOTE: Bones

			VertexBoneInfo vb = vertex_information[i];
			data.push_back(vb.bone_index[0]);
			data.push_back(vb.bone_index[1]);
			data.push_back(vb.bone_index[2]);
			data.push_back(vb.bone_index[3]);
			data.CopyFromPtr(vb.weights, 4 * sizeof(float));

		}

		VertexBuffer vbo;
		vbo.lbo = l;
		vbo.size_bytes = data.size() * sizeof(real);
		vbo.flags = VertexFlags::READ_WRITE; // Optional;

		CreateVertexBuffer(&vbo);
		WriteBufferData(&vbo, data, 0);

		IndexBuffer ibo;
		ibo.index_count = (uint32)indices.size();
		ibo.size_bytes = indices.size() * sizeof(uint32);
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


























	Matrix4f ToMatrix4fgawd(const aiMatrix4x4 *ai_mat)
	{
		Assert(sizeof(aiMatrix4x4) == sizeof(Matrix4f));
		uint32 size = sizeof(Matrix4f);

		Matrix4f a;
		memcpy((void*)&a, (void*)ai_mat, size);

		return a;
	}

	Mat4 ToMatrix4fgawd(const Matrix4f *ai_mat)
	{
		Assert(sizeof(aiMatrix4x4) == sizeof(Matrix4f));
		uint32 size = sizeof(Matrix4f);

		Matrix4f ai = ai_mat->Transpose();
		Mat4 a;
		memcpy((void*)&a, (void *)&ai, size);

		return a;
	}

	Mat4 ToMatrix4ffrom3x3(const aiMatrix3x3 *ai_mat)
	{
		uint32 size = sizeof(aiMatrix3x3);

		Mat4 a(1);
		a.row0 = Vec4(ai_mat->a1, ai_mat->a2, ai_mat->a3, 0);
		a.row1 = Vec4(ai_mat->b1, ai_mat->b2, ai_mat->b3, 0);
		a.row2 = Vec4(ai_mat->c1, ai_mat->c2, ai_mat->c3, 0);

		return a;
	}



	void Matrix4f::InitScaleTransform(float ScaleX, float ScaleY, float ScaleZ)
	{
		m[0][0] = ScaleX; m[0][1] = 0.0f;   m[0][2] = 0.0f;   m[0][3] = 0.0f;
		m[1][0] = 0.0f;   m[1][1] = ScaleY; m[1][2] = 0.0f;   m[1][3] = 0.0f;
		m[2][0] = 0.0f;   m[2][1] = 0.0f;   m[2][2] = ScaleZ; m[2][3] = 0.0f;
		m[3][0] = 0.0f;   m[3][1] = 0.0f;   m[3][2] = 0.0f;   m[3][3] = 1.0f;
	}

	void Matrix4f::InitRotateTransform(float RotateX, float RotateY, float RotateZ)
	{
		Matrix4f rx, ry, rz;

		const float x = DegToRad(RotateX);
		const float y = DegToRad(RotateY);
		const float z = DegToRad(RotateZ);

		rx.m[0][0] = 1.0f; rx.m[0][1] = 0.0f; rx.m[0][2] = 0.0f; rx.m[0][3] = 0.0f;
		rx.m[1][0] = 0.0f; rx.m[1][1] = cosf(x); rx.m[1][2] = -sinf(x); rx.m[1][3] = 0.0f;
		rx.m[2][0] = 0.0f; rx.m[2][1] = sinf(x); rx.m[2][2] = cosf(x); rx.m[2][3] = 0.0f;
		rx.m[3][0] = 0.0f; rx.m[3][1] = 0.0f; rx.m[3][2] = 0.0f; rx.m[3][3] = 1.0f;

		ry.m[0][0] = cosf(y); ry.m[0][1] = 0.0f; ry.m[0][2] = -sinf(y); ry.m[0][3] = 0.0f;
		ry.m[1][0] = 0.0f; ry.m[1][1] = 1.0f; ry.m[1][2] = 0.0f; ry.m[1][3] = 0.0f;
		ry.m[2][0] = sinf(y); ry.m[2][1] = 0.0f; ry.m[2][2] = cosf(y); ry.m[2][3] = 0.0f;
		ry.m[3][0] = 0.0f; ry.m[3][1] = 0.0f; ry.m[3][2] = 0.0f; ry.m[3][3] = 1.0f;

		rz.m[0][0] = cosf(z); rz.m[0][1] = -sinf(z); rz.m[0][2] = 0.0f; rz.m[0][3] = 0.0f;
		rz.m[1][0] = sinf(z); rz.m[1][1] = cosf(z); rz.m[1][2] = 0.0f; rz.m[1][3] = 0.0f;
		rz.m[2][0] = 0.0f; rz.m[2][1] = 0.0f; rz.m[2][2] = 1.0f; rz.m[2][3] = 0.0f;
		rz.m[3][0] = 0.0f; rz.m[3][1] = 0.0f; rz.m[3][2] = 0.0f; rz.m[3][3] = 1.0f;

		*this = rz * ry * rx;
	}

	void Matrix4f::InitTranslationTransform(float x, float y, float z)
	{
		m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = x;
		m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = y;
		m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = z;
		m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
	}

	float Matrix4f::Determinant() const
	{
		return m[0][0] * m[1][1] * m[2][2] * m[3][3] - m[0][0] * m[1][1] * m[2][3] * m[3][2] + m[0][0] * m[1][2] * m[2][3] * m[3][1] - m[0][0] * m[1][2] * m[2][1] * m[3][3]
			+ m[0][0] * m[1][3] * m[2][1] * m[3][2] - m[0][0] * m[1][3] * m[2][2] * m[3][1] - m[0][1] * m[1][2] * m[2][3] * m[3][0] + m[0][1] * m[1][2] * m[2][0] * m[3][3]
			- m[0][1] * m[1][3] * m[2][0] * m[3][2] + m[0][1] * m[1][3] * m[2][2] * m[3][0] - m[0][1] * m[1][0] * m[2][2] * m[3][3] + m[0][1] * m[1][0] * m[2][3] * m[3][2]
			+ m[0][2] * m[1][3] * m[2][0] * m[3][1] - m[0][2] * m[1][3] * m[2][1] * m[3][0] + m[0][2] * m[1][0] * m[2][1] * m[3][3] - m[0][2] * m[1][0] * m[2][3] * m[3][1]
			+ m[0][2] * m[1][1] * m[2][3] * m[3][0] - m[0][2] * m[1][1] * m[2][0] * m[3][3] - m[0][3] * m[1][0] * m[2][1] * m[3][2] + m[0][3] * m[1][0] * m[2][2] * m[3][1]
			- m[0][3] * m[1][1] * m[2][2] * m[3][0] + m[0][3] * m[1][1] * m[2][0] * m[3][2] - m[0][3] * m[1][2] * m[2][0] * m[3][1] + m[0][3] * m[1][2] * m[2][1] * m[3][0];
	}

	Matrix4f& Matrix4f::Inverse()
	{
		// Compute the reciprocal determinant
		float det = Determinant();
		if (det == 0.0f)
		{
			// Matrix not invertible. Setting all elements to nan is not really
			// correct in a mathematical sense but it is easy to debug for the
			// programmer.
			/*const float nan = std::numeric_limits<float>::quiet_NaN();
			*this = Matrix4f(
				nan,nan,nan,nan,
				nan,nan,nan,nan,
				nan,nan,nan,nan,
				nan,nan,nan,nan);*/
	
			return *this;
		}

		float invdet = 1.0f / det;

		Matrix4f res;
		res.m[0][0] = invdet * (m[1][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) + m[1][2] * (m[2][3] * m[3][1] - m[2][1] * m[3][3]) + m[1][3] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]));
		res.m[0][1] = -invdet * (m[0][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) + m[0][2] * (m[2][3] * m[3][1] - m[2][1] * m[3][3]) + m[0][3] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]));
		res.m[0][2] = invdet * (m[0][1] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) + m[0][2] * (m[1][3] * m[3][1] - m[1][1] * m[3][3]) + m[0][3] * (m[1][1] * m[3][2] - m[1][2] * m[3][1]));
		res.m[0][3] = -invdet * (m[0][1] * (m[1][2] * m[2][3] - m[1][3] * m[2][2]) + m[0][2] * (m[1][3] * m[2][1] - m[1][1] * m[2][3]) + m[0][3] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]));
		res.m[1][0] = -invdet * (m[1][0] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) + m[1][2] * (m[2][3] * m[3][0] - m[2][0] * m[3][3]) + m[1][3] * (m[2][0] * m[3][2] - m[2][2] * m[3][0]));
		res.m[1][1] = invdet * (m[0][0] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) + m[0][2] * (m[2][3] * m[3][0] - m[2][0] * m[3][3]) + m[0][3] * (m[2][0] * m[3][2] - m[2][2] * m[3][0]));
		res.m[1][2] = -invdet * (m[0][0] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) + m[0][2] * (m[1][3] * m[3][0] - m[1][0] * m[3][3]) + m[0][3] * (m[1][0] * m[3][2] - m[1][2] * m[3][0]));
		res.m[1][3] = invdet * (m[0][0] * (m[1][2] * m[2][3] - m[1][3] * m[2][2]) + m[0][2] * (m[1][3] * m[2][0] - m[1][0] * m[2][3]) + m[0][3] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]));
		res.m[2][0] = invdet * (m[1][0] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) + m[1][1] * (m[2][3] * m[3][0] - m[2][0] * m[3][3]) + m[1][3] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]));
		res.m[2][1] = -invdet * (m[0][0] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) + m[0][1] * (m[2][3] * m[3][0] - m[2][0] * m[3][3]) + m[0][3] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]));
		res.m[2][2] = invdet * (m[0][0] * (m[1][1] * m[3][3] - m[1][3] * m[3][1]) + m[0][1] * (m[1][3] * m[3][0] - m[1][0] * m[3][3]) + m[0][3] * (m[1][0] * m[3][1] - m[1][1] * m[3][0]));
		res.m[2][3] = -invdet * (m[0][0] * (m[1][1] * m[2][3] - m[1][3] * m[2][1]) + m[0][1] * (m[1][3] * m[2][0] - m[1][0] * m[2][3]) + m[0][3] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]));
		res.m[3][0] = -invdet * (m[1][0] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]) + m[1][1] * (m[2][2] * m[3][0] - m[2][0] * m[3][2]) + m[1][2] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]));
		res.m[3][1] = invdet * (m[0][0] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]) + m[0][1] * (m[2][2] * m[3][0] - m[2][0] * m[3][2]) + m[0][2] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]));
		res.m[3][2] = -invdet * (m[0][0] * (m[1][1] * m[3][2] - m[1][2] * m[3][1]) + m[0][1] * (m[1][2] * m[3][0] - m[1][0] * m[3][2]) + m[0][2] * (m[1][0] * m[3][1] - m[1][1] * m[3][0]));
		res.m[3][3] = invdet * (m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) + m[0][1] * (m[1][2] * m[2][0] - m[1][0] * m[2][2]) + m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]));
		*this = res;

		return *this;
	}













}
