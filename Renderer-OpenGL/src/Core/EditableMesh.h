#pragma once
#include "Core.h"
#include "OpenGL/OpenGl.h"
#include "Animation.h"

namespace cm
{
	#define MAX_VERTEX_BONE_COUNT 4
	struct Vertex
	{
		Vec3f position = Vec3f(0);
		Vec3f normal = Vec3f(0);
		Vec3f tangent = Vec3f(0);
		Vec3f bitangent = Vec3f(0);
		Vec3f colour = Vec3f(0);
		Vec3f texture_coord = Vec3f(0);
		uint32 next = 0;		
		uint32 bone_index[MAX_VERTEX_BONE_COUNT] = { 0, 0, 0, 0 };
		real32 bone_weights[MAX_VERTEX_BONE_COUNT] = { 0, 0, 0, 0 };
	};
	   	  
	struct InterMeshConfig
	{
		bool32 normals = true;
		bool32 texture_coords = true;
		bool32 tanget_bitangets = true;
		bool32 colours = false;
		bool32 faces = false;
		bool32 bones = false;
	};

	struct InterMesh
	{
		Array<Vertex> vertices;
		Array<uint32> indices;
		InterMeshConfig config;
		String name;
	};
	
	void CreateGlMesh(InterMesh *mesh, GLMesh *glmesh, bool free = false);


	struct Edge
	{
		Vertex vertices[2] = {};
		real32 length = 0;
	};

	struct Triangle
	{
		real32 area;
		Vec3f normal;
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
			Vec3f edges[3];
			struct
			{
				Vec3f edge1;
				Vec3f edge2;
				Vec3f edge3;
			};
		};
		Vec3f normal;
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
