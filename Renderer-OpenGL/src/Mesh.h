#pragma once
#include "Core.h"
#include "OpenGL/OpenGlRenderer.h"
namespace cm
{
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


	


	class EditableMesh
	{
	public:
		DynaArray<Vertex> vertices;
		DynaArray<IndexedTriangle> tris;

	public:
		Mesh CreateMesh();
		void LoadMesh(Mesh mesh);
		void AddTrianlge(const Vec3 &pos1, const Vec3 &pos2, const Vec3 &pos3);
		void AddTextureCoords(const Vec3 &t1, const Vec3 &t2, const Vec3 &t3);
		void AddTrianlge(const Vertex &p1, const Vertex &p2, const Vertex &p3);
		void ConvertToPNT(DynaArray<float> *data, DynaArray<uint32> *index) const;
		void RecaluclateNormals();
		void FuseVertices(float tollerance = FLOATING_POINT_ERROR_PRESCION);
		
		void ConvertToOther(DynaArray<float> *data, DynaArray<uint32> *index) const; //<-- Just for testing purposes
		void SetColour(const Vec3 colour);
		
	};

}
