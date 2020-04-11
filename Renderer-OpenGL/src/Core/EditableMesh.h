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
		Mat4 current_transform;
		Mat4 inverse_bind_transform;
		Mat4 node_transform_matrix;
		// @NOTE: Assume there is only one parent. -1 is an invaild value
		int32 parent_index = -1;			
		// @NOTE: Child bones indices. -1 is an invaild value
		std::string name = "Empty";
		DynaArray<int32> child_indices;		
	};

	class AnimationFrames
	{
	public:
		int32 bone_index = -1;
		std::string name;
		DynaArray<real32> postime;
		DynaArray<Vec3> poskeys;

	 	DynaArray<real32> rottime;
		DynaArray<Quat> rotkeys;

		DynaArray<real32> scltime;
		DynaArray<Vec3> sclkeys;
	};

	class Animation
	{
	public:		
		float duration = 0;	
		float ticks_per_second = 0;
		DynaArray<AnimationFrames> frames;
		DynaArray<Bone> *working_bones;
		void Play(real time, DynaArray<Bone> *bones);
		void AnimateBones(const real &animation_time, Bone *bone, const Mat4 &parent_transform);
		
		int32 GetAnimationChannel(const std::string &bone_name);

		Vec3 CalculateInterpolatedScaling(const real32 &animation_time, const int32 &keyset_index);
		Vec3 CalculateInterpolatedPosition(const real32 &animation_time, const int32 &keyset_index);
		Quat CalculateInterpolatedRotation(const real32 &animation_time, const int32 &keyset_index);

		int32 FindPosition(const real32 &animation_time, const int32 &keyset_index);
		int32 FindRotation(const real32 &animation_time, const int32 &keyset_index);
		int32 FindScaling(const real32 &animation_time, const int32 &keyset_index);
	};

	class AnimationController
	{
	public:
		real current_time;

		// @NOTE: We reserve the [0]/0th index to be the root bone. This root bone does not actually exist in the mesh
		// @NOTE: It is simply a place holder to make all other calculations easiers

		DynaArray<Bone> bones;
		DynaArray<Animation> animations;
		
		void Play(const std::string &name);
		void Play(uint32 animation_index);

		Mat4 global_inverse_transform;
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
	public:
		DynaArray<Vertex> vertices;
		DynaArray<uint32> indices;
		DynaArray<IndexedTriangle> tris;

		std::string name;
		
		Animation animation;
		AnimationController ac;
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
