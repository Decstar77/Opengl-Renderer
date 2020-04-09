#pragma once
#include "Core.h"
#include "OpenGL/OpenGl.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace cm
{


	struct Vertex
	{
		Vec3 position = Vec3(0);
		Vec3 normal = Vec3(0);
		Vec3 tanget = Vec3(0);
		Vec3 bitanget = Vec3(0);
		Vec3 colour = Vec3(0);
		Vec3 texture_coord = Vec3(0);
		uint32 next = 0;
		uint32 bone_index[4] = { 0, 0, 0, 0 };
		float bone_weights[4] = { 0, 0, 0, 0 };
	};

	struct Bone
	{
	public:
		std::string name = "Empty";
		Mat4 inverse_bind_pose;
		Mat4 node_transform;
		Bone *child;
	};

	class AnimationChannel
	{
	public:
		Bone *bone;

		std::string name;
		float postime[250];
		Vec3 poskeys[250];

		float rottime[250];
		Quat rotkeys[250];

		float scltime[250];
		Vec3 sclkeys[250];
	};

	class Animation
	{
	public:
		float duration;
		float tick_per_second;
		Bone bones[3]; // @NOTE: This is mesh, maybe put in animation controller
		AnimationChannel channels[2];
		Mat4 global_inverse_transform;
	};

	class Matrix4f
	{
	public:
		float m[4][4];

		Matrix4f()
		{
		}

		// constructor from Assimp matrix
		Matrix4f(const aiMatrix4x4& AssimpMatrix)
		{
			m[0][0] = AssimpMatrix.a1; m[0][1] = AssimpMatrix.a2; m[0][2] = AssimpMatrix.a3; m[0][3] = AssimpMatrix.a4;
			m[1][0] = AssimpMatrix.b1; m[1][1] = AssimpMatrix.b2; m[1][2] = AssimpMatrix.b3; m[1][3] = AssimpMatrix.b4;
			m[2][0] = AssimpMatrix.c1; m[2][1] = AssimpMatrix.c2; m[2][2] = AssimpMatrix.c3; m[2][3] = AssimpMatrix.c4;
			m[3][0] = AssimpMatrix.d1; m[3][1] = AssimpMatrix.d2; m[3][2] = AssimpMatrix.d3; m[3][3] = AssimpMatrix.d4;
		}

		Matrix4f(const aiMatrix3x3& AssimpMatrix)
		{
			m[0][0] = AssimpMatrix.a1; m[0][1] = AssimpMatrix.a2; m[0][2] = AssimpMatrix.a3; m[0][3] = 0.0f;
			m[1][0] = AssimpMatrix.b1; m[1][1] = AssimpMatrix.b2; m[1][2] = AssimpMatrix.b3; m[1][3] = 0.0f;
			m[2][0] = AssimpMatrix.c1; m[2][1] = AssimpMatrix.c2; m[2][2] = AssimpMatrix.c3; m[2][3] = 0.0f;
			m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
		}

		Matrix4f(float a00, float a01, float a02, float a03,
			float a10, float a11, float a12, float a13,
			float a20, float a21, float a22, float a23,
			float a30, float a31, float a32, float a33)
		{
			m[0][0] = a00; m[0][1] = a01; m[0][2] = a02; m[0][3] = a03;
			m[1][0] = a10; m[1][1] = a11; m[1][2] = a12; m[1][3] = a13;
			m[2][0] = a20; m[2][1] = a21; m[2][2] = a22; m[2][3] = a23;
			m[3][0] = a30; m[3][1] = a31; m[3][2] = a32; m[3][3] = a33;
		}


		Matrix4f Transpose() const
		{
			Matrix4f n;

			for (unsigned int i = 0; i < 4; i++) {
				for (unsigned int j = 0; j < 4; j++) {
					n.m[i][j] = m[j][i];
				}
			}

			return n;
		}


		inline void InitIdentity()
		{
			m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
			m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
			m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = 0.0f;
			m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
		}

		inline Matrix4f operator*(const Matrix4f& Right) const
		{
			Matrix4f Ret;

			for (unsigned int i = 0; i < 4; i++) {
				for (unsigned int j = 0; j < 4; j++) {
					Ret.m[i][j] = m[i][0] * Right.m[0][j] +
						m[i][1] * Right.m[1][j] +
						m[i][2] * Right.m[2][j] +
						m[i][3] * Right.m[3][j];
				}
			}

			return Ret;
		}

		operator const float*() const
		{
			return &(m[0][0]);
		}

		void Print() const
		{
			Matrix4f m = *this;
			m = m.Transpose();
			const GLfloat *f =  (const GLfloat*)m;
			std::string space = "            ";
			space = " ";
			for (int i = 0; i < 16; i++)
				std::cout << f[i] << space;
			std::cout << std::endl;
		}

		float Determinant() const;

		Matrix4f& Inverse();

		void InitScaleTransform(float ScaleX, float ScaleY, float ScaleZ);
		void InitRotateTransform(float RotateX, float RotateY, float RotateZ);
		void InitTranslationTransform(float x, float y, float z);
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

	struct BoneInfo
	{
		std::string name;
		Matrix4f bone_space_to_mesh_space;
		Matrix4f ft;
	};

	class EditableMesh
	{
	public:
		DynaArray<Vertex> vertices;
		DynaArray<uint32> indices;
		DynaArray<IndexedTriangle> tris;

		std::string name;
		
		Animation animation;

		bool has_positions = false;
		bool has_normals = false;
		bool has_texture_coords = false;
		bool has_tanget_bitangets = false;
		bool has_colours = false;
		bool has_faces = false;

	public:
		GLMesh CreateAnimMesh();
		GLMesh CreateAnimMesh(const std::vector<VertexBoneInfo> & vertex_information);
		GLMesh CreateMesh(bool tangets);
		void LoadMesh(GLMesh mesh);
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
