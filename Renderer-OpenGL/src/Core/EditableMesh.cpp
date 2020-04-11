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


	void AnimationController::Play(const std::string &name)
	{
		for (uint32 i = 0; i < animations.size(); i++)
		{


		}
	}

	void AnimationController::Play(uint32 animation_index)
	{
		Assert(animation_index < animations.size());
		Assert(animation_index >= 0);

		Animation * anim = &animations[animation_index];
		anim->Play(current_time, &bones);

		for (int32 i = 0; i < bones.size(); i++)
		{
			//bones[i].current_transform = bones[i].current_transform * global_inverse_transform;
		}

	}

	void Animation::Play(real time,  DynaArray<Bone> *bones)
	{
		// @TODO: Time calcs
		real32 time_in_ticks = time * ticks_per_second;
		real32 animation_time = fmod(time_in_ticks, duration);
	
		working_bones = bones;
		AnimateBones(animation_time, &bones->at(0), Mat4(1));
	}

	void Animation::AnimateBones(const real &animation_time, Bone *bone, const Mat4 &parent_transform)
	{
		std::string bone_name = bone->name;
		Mat4 node_transform = bone->node_transform_matrix;

		// @NOTE: Returns -1 if nothing is found
		int32 keyset_index = GetAnimationChannel(bone_name);

		if (keyset_index != -1)
		{
			Assert(keyset_index < frames.size());
			Assert(keyset_index >= 0);
			
			Vec3 pos	= CalculateInterpolatedPosition(animation_time, keyset_index); 
			Quat q		= CalculateInterpolatedRotation(animation_time, keyset_index);
			Vec3 scl	= CalculateInterpolatedScaling(animation_time, keyset_index);

			Mat4 scaleM = ScaleCardinal(Mat4(1), scl);
			Mat4 rotM	= QuatToMat4(q);
			Mat4 tranM	= Translate(Mat4(1), pos);

			node_transform = scaleM * rotM * tranM;
		}

		Mat4 global_transform = node_transform * parent_transform;
		
		bone->current_transform = bone->inverse_bind_transform * global_transform;

		//std::cout << bone_name << std::endl;
		//std::cout << "parent_transform: ";
		//Print(parent_transform);
		//std::cout << "node_transform: ";
		//Print(node_transform);
		//std::cout << "Global_transform: ";
		//Print(global_transform);
		//std::cout << "bind_transform: ";
		//Print(bone->inverse_bind_transform);
		//std::cout << "final_transform: ";
		//Print(bone->current_transform);
		//std::cout << std::endl;


		for (uint32 i = 0; i < bone->child_indices.size(); i++)
		{
			uint32 next_bone_index = bone->child_indices.at(i);
			Bone *next_bone = &working_bones->at(next_bone_index);
			AnimateBones(animation_time, next_bone, global_transform);
		}
	}

	int32 Animation::GetAnimationChannel(const std::string &bone_name)
	{		
		for (uint32 i = 0; i < frames.size(); i++)
		{
			if (frames[i].name == bone_name)
				return i;
		}
		return -1;
	}


	cm::Vec3 Animation::CalculateInterpolatedPosition(const real32 &animation_time, const int32 &keyset_index)
	{
		const AnimationFrames &keyset = frames[keyset_index];

		// TODO: Make sure the keyset has more than one key

		int32 position_index = FindPosition(animation_time, keyset_index);
		int32 next_position_index = position_index + 1; 		
		
		real32 delta_time = keyset.postime[next_position_index] - keyset.postime[position_index];

		real32 factor = (animation_time - keyset.postime[position_index]) / delta_time;

		if (factor < 0 || factor >= 1)
		{
			//LOG("Warning clamping factor, factor = " << factor);
		}
		factor = Clamp(factor, 0.f, 1.f);


		Vec3 start = keyset.poskeys[position_index];
		Vec3 end = keyset.poskeys[next_position_index];

		Vec3 result = Lerp(start, end, factor);

		return result;
	}

	cm::Quat Animation::CalculateInterpolatedRotation(const real32 &animation_time, const int32 &keyset_index)
	{		
		const AnimationFrames &keyset = frames[keyset_index];

		// TODO: Make sure the keyset has more than one key

		int32 rotation_index = FindRotation(animation_time, keyset_index);
		int32 next_rotation_index = rotation_index + 1; 

		real32 delta_time = keyset.rottime[next_rotation_index] - keyset.rottime[rotation_index];

		real32 factor = (animation_time - keyset.rottime[rotation_index]) / delta_time;

		if (factor < 0 || factor >= 1)
		{
			//LOG("Warning clamping factor, factor = " << factor);
		}
		factor = Clamp(factor, 0.f, 1.f);
		
		Quat start = keyset.rotkeys[rotation_index];
		Quat end = keyset.rotkeys[next_rotation_index];
		
		Quat result = Slerp(start, end, factor);

		return result;
	}


	cm::Vec3 Animation::CalculateInterpolatedScaling(const real32 &animation_time, const int32 &keyset_index)
	{
		const AnimationFrames &keyset = frames[keyset_index];

		// TODO: Make sure the keyset has more than one key

		int32 scaling_index = FindScaling(animation_time, keyset_index);
		int32 next_scaling_index = scaling_index + 1;

		real32 delta_time = keyset.scltime[next_scaling_index] - keyset.scltime[scaling_index];

		real32 factor = (animation_time - keyset.scltime[scaling_index]) / delta_time;

		if (factor < 0 || factor >= 1)
		{
			//LOG("Warning clamping factor, factor = " << factor);
		}
		factor = Clamp(factor, 0.f, 1.f);


		Vec3 start = keyset.sclkeys[scaling_index];
		Vec3 end = keyset.sclkeys[next_scaling_index];

		Vec3 result = Lerp(start, end, factor);
		return result;
	}

	int32 Animation::FindPosition(const real32 &animation_time, const int32 &keyset_index)
	{
		const AnimationFrames &keyset = frames[keyset_index];

		for (uint32 i = 0; i < keyset.postime.size(); i++)
		{
			if (animation_time < keyset.postime[i + 1])
			{
				return i;
			}
		}

		Assert(0);
		return -1;
	}

	int32 Animation::FindRotation(const real32 &animation_time, const int32 &keyset_index)
	{
		const AnimationFrames &keyset = frames[keyset_index];

		for (uint32 i = 0; i < keyset.rottime.size(); i++)
		{
			if (animation_time < keyset.rottime[i + 1])
			{
				return i;
			}
		}

		Assert(0);
		return -1;
	}

	int32 Animation::FindScaling(const real32 &animation_time, const int32 &keyset_index)
	{
		const AnimationFrames &keyset = frames[keyset_index];
		
		for (uint32 i = 0; i < keyset.scltime.size(); i++)
		{
			if (animation_time < keyset.scltime[i + 1])
			{
				return i; 
			}
		}

		Assert(0);
		return -1;
	}

}
