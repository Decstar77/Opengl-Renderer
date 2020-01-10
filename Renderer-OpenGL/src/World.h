#pragma once
#include "OpenGL/OpenGlRenderer.h"

namespace cm
{
	class CameraController
	{		
		Camera main_camera;
	public:
		CameraController() {};
		CameraController(Camera cam) : main_camera(cam) {}
		~CameraController() {};

		void Rotate();
		void Movement();

	};

	struct Vertex{
		Vec3 position;
		Vec3 normal;
		Vec3 texture_coord;
	};


	//@Redo: Maybe we make this a pod struct and have global functions. This could also be inside the math lib
	class Triangle
	{
	public:
		union
		{
			uint32 index[3];
			struct
			{
				uint32 v1;
				uint32 v2;
				uint32 v3;
			};
		};
		union
		{
			Vec3 edges[3];
			struct
			{
				Vec3 edge1;
				Vec3 edge2;
				Vec3 edge3;
			};			
		};
		Vec3 normal;
	public:
		Triangle() {}	//Don't know why but I need to declare these explicitly for unoin.
		~Triangle() {}	//Don't know why but I need to declare these explicitly for unoin.
		inline real GetArea() { return Mag(Cross(edge1, edge2)) / 2; } 
		inline void CalculateNormal() { normal = Normalize(Cross(edge1, edge2)); }
	};


	//MeshGenerator mesh_gen;
	//mesh_gen.AddTrianlge(Vec3(0), Vec3(0, 1, 0), Vec3(1, 0, 0));
	//DynaArray<float> temp_gen_va;
	//DynaArray<uint32> temp_gen_ib;
	//mesh_gen.ConvertPNT(&temp_gen_va, &temp_gen_ib);
	//VertexBuffer mesh_gen_vbo = CreateBuffer<VertexBuffer>(temp_gen_va.size() * sizeof(float), VertexFlags::READ_WRITE);
	//WriteBufferData(mesh_gen_vbo, temp_gen_va, 0);
	//IndexBuffer mesh_gen_ibo = CreateBuffer<IndexBuffer>(temp_gen_ib.size() * sizeof(uint32), VertexFlags::READ_WRITE);
	//WriteBufferData(mesh_gen_ibo, temp_gen_ib, 0);
	//mesh_gen_ibo.index_count = temp_gen_ib.size();
	//mesh_gen_ibo.size_bytes = temp_gen_ib.size() * sizeof(float);


	//BufferLayout mesh_lbo = PNT_VBO_LAYOUT
	//	VertexArray mesh_gen_vao = CreateVertexArray(mesh_lbo, mesh_gen_vbo);

	//Mesh mesh_gen_mesh;
	//mesh_gen_mesh.vertices = &temp_gen_va;
	//mesh_gen_mesh.vao = vao;
	//mesh_gen_mesh.ibo = mesh_gen_ibo;

	class MeshGenerator
	{
	public:
		DynaArray<Vertex> vertices;		
		DynaArray<Triangle> tris;		

	public:

		void AddTrianlge(const Vec3 &pos1, const Vec3 &pos2, const Vec3 &pos3)
		{
			Triangle tri;
			Vertex p1;
			p1.position = pos1;
			vertices.push_back(p1);
			tri.v1 = static_cast<uint32>(vertices.size()) - 1;

			Vertex p2;
			p2.position = pos2;
			vertices.push_back(p2);
			tri.v2 = static_cast<uint32>(vertices.size()) - 1;

			Vertex p3;
			p3.position = pos3;
			vertices.push_back(p3);
			tri.v3 = static_cast<uint32>(vertices.size()) - 1;

			tri.edge1 = pos2 - pos1;
			tri.edge2 = pos3 - pos2;
			tri.edge3 = pos1 - pos3;

			tri.CalculateNormal();
			tris.push_back(tri);
		}

		void AddTrianlge(const Vertex &p1, const Vertex &p2, const Vertex &p3)
		{
			vertices.push_back(p1);
			vertices.push_back(p2);
			vertices.push_back(p3);
			uint32 count_index = static_cast<uint32>(vertices.size()) - 1;
			Triangle tri;

			tri.v1 = count_index - 2;
			tri.v2 = count_index - 1;
			tri.v3 = count_index;

			tri.edge1 = p2.position - p1.position;
			tri.edge2 = p3.position - p2.position;
			tri.edge3 = p1.position - p3.position;

			tri.CalculateNormal();
			tris.push_back(tri);
		}

		void RecaluclateNormals()
		{
			for (uint32 i; i < static_cast<uint32>(tris.size()); i++)
			{
				tris[i].CalculateNormal();
			}
		}

		void ConvertPNT(DynaArray<float> *data, DynaArray<uint32> *index) const
		{
			//@Speed: Prehaps resize arr before hand
			uint32 index_counter = 0;
			for (uint32 i = 0; i < static_cast<uint32>(tris.size()); i++)
			{
				Triangle tri = tris[i];
				for (int32 p = 0; p < 3; p++)
				{
					Vertex vert = vertices[tri.index[p]];
					data->CopyFromPtr(vert.position.arr, 3 * sizeof(float));
					data->CopyFromPtr(vert.normal.arr, 3 * sizeof(float));
					data->push_back(vert.texture_coord.x);
					data->push_back(vert.texture_coord.y);
					index->push_back(index_counter++);
				}
			}
		}
	
	};


}