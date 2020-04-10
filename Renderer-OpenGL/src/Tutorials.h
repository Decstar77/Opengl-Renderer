#pragma once



#include <vector>
#include <string>
#include <map>
#include <iostream>
#include "glm\gtc\quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "assimp\Importer.hpp"
#include "assimp\scene.h"
#include "assimp\postprocess.h"
#include "Core.h"


using namespace std;

namespace cm
{
	namespace tut
	{
		
		typedef unsigned int GLuint;
		typedef unsigned int uint;
#define NUM_BONES_PER_VEREX 4

		struct Vertex
		{
			glm::vec3 position;
			glm::vec3 normal;
			glm::vec2 text_coords;
		};

		struct Texture
		{
			GLuint id;
			string type;
			aiString path;
		};

		struct BoneMatrix
		{
			aiMatrix4x4 offset_matrix;
			aiMatrix4x4 final_world_transform;

		};

		struct VertexBoneData
		{
			uint ids[NUM_BONES_PER_VEREX];   // we have 4 bone ids for EACH vertex & 4 weights for EACH vertex
			float weights[NUM_BONES_PER_VEREX];

			VertexBoneData()
			{
				memset(ids, 0, sizeof(ids));    // init all values in array = 0
				memset(weights, 0, sizeof(weights));
			}

			void addBoneData(uint bone_id, float weight);
		};

		class Mesh
		{
		public:
			Mesh(vector<Vertex> vertic, vector<GLuint> ind, vector<Texture> textur, vector<VertexBoneData> bone_id_weights);
			Mesh() {};
			~Mesh();

			// Render mesh

			public:
			//Mesh data
			vector<Vertex> vertices;
			vector<GLuint> indices;
			vector<Texture> textures;
			vector<VertexBoneData> bones_id_weights_for_each_vertex;

			//buffers
			GLuint VAO;
			GLuint VBO_vertices;
			GLuint VBO_bones;
			GLuint EBO_indices;

			//inititalize buffers
		};


		class Model
		{
		public:
			Model();
			~Model();
			static const uint MAX_BONES = 100;


			void loadModel(const string& path);
			void playSound();
			void showNodeName(aiNode* node);

			glm::mat4 aiToGlm(aiMatrix4x4 ai_matr);
			aiQuaternion nlerp(aiQuaternion a, aiQuaternion b, float blend); // super super n lerp =)

		public:
			Assimp::Importer import;
			const aiScene* scene;
			vector<Mesh> meshes; // one mesh in one object
			string directory;

			map<string, uint> m_bone_mapping; // maps a bone name and their index
			uint m_num_bones = 0;
			vector<BoneMatrix> m_bone_matrices;
			aiMatrix4x4 m_global_inverse_transform;

			GLuint m_bone_location[MAX_BONES];
			float ticks_per_second = 0.0f;

			void processNode(aiNode* node, const aiScene* scene);
			Mesh processMesh(aiMesh* mesh, const aiScene* scene);


			uint findPosition(float p_animation_time, const aiNodeAnim* p_node_anim);
			uint findRotation(float p_animation_time, const aiNodeAnim* p_node_anim);
			uint findScaling(float p_animation_time, const aiNodeAnim* p_node_anim);
			const aiNodeAnim* findNodeAnim(const aiAnimation* p_animation, const string p_node_name);
			// calculate transform matrix
			aiVector3D calcInterpolatedPosition(float p_animation_time, const aiNodeAnim* p_node_anim);
			aiQuaternion calcInterpolatedRotation(float p_animation_time, const aiNodeAnim* p_node_anim);
			aiVector3D calcInterpolatedScaling(float p_animation_time, const aiNodeAnim* p_node_anim);

			void readNodeHierarchy(float p_animation_time, const aiNode* p_node, const aiMatrix4x4 parent_transform);
			void boneTransform(double time_in_sec, vector<aiMatrix4x4>& transforms);

			// rotate Head
			glm::quat rotate_head_xz = glm::quat(cos(glm::radians(0.0f)), sin(glm::radians(0.0f)) * glm::vec3(1.0f, 0.0f, 0.0f)); // this quad do nothingggggg!!!!!

		};


		void Print(const glm::mat4 &mat);










	}

}