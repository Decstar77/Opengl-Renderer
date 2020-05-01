#pragma once
#include "Core.h"
#include "OpenGL/OpenGl.h"
#include "Animation.h"

namespace cm
{
	#define MAX_VERTEX_BONE_COUNT 4
	struct Vertex
	{
		Vec3 position = Vec3(0);
		Vec3 normal = Vec3(0);
		Vec3 tangent = Vec3(0);
		Vec3 bitangent = Vec3(0);
		Vec3 colour = Vec3(0);
		Vec3 texture_coord = Vec3(0);
		uint32 next = 0;		
		uint32 bone_index[MAX_VERTEX_BONE_COUNT] = { 0, 0, 0, 0 };
		float bone_weights[MAX_VERTEX_BONE_COUNT] = { 0, 0, 0, 0 };
	};

	struct Edge
	{
		Vertex vertices[2] = {};
		real32 length = 0;
	};

	struct Triangle
	{
		real32 area;
		Vec3 normal;
		Vertex vertices[3];
		Edge edges[2];
	};

	class IndexedTriangle
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
		IndexedTriangle() {}
		~IndexedTriangle() {}		
		inline real GetArea() { return Mag(Cross(edge1, edge2)) / 2; }
		inline void CalculateNormal() { normal = Normalize(Cross(edge1, edge2)); }
	};

	struct VertexBoneInfo
	{
		uint32 bone_index[4];
		float weights[4];
		void AddBoneData(float Weight, uint32 id)
		{
			for (uint32 i = 0; i < 4; i++) {
				if (weights[i] == 0.0) {
					bone_index[i] = id;
					weights[i] = Weight;
					return;
				}
			}
		}
		uint32 next_space = 0;
	};



	class EditableMesh
	{
		//https://answers.unity.com/questions/228841/dynamically-combine-verticies-that-share-the-same.html
	public:
		std::vector<Vertex> vertices;
		std::vector<uint32> indices;

		String name;		
		bool has_positions = false;
		bool has_normals = false;
		bool has_texture_coords = false;
		bool has_tanget_bitangets = false;
		bool has_colours = false;
		bool has_faces = false;

	public:
		GLMesh CreateAnimMesh();
		GLMesh CreateMesh(bool tangets);
		GLMesh CreateMesh();
		void LoadMesh(GLMesh mesh);
		void ConvertToPNT(std::vector<float> *data, std::vector<uint32> *index) const;

		void ConvertToOther(std::vector<float> *data, std::vector<uint32> *index) const; //<-- Just for testing purposes

	};

}
