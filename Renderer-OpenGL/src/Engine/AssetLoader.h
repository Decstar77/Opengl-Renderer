#pragma once
#include "Core.h"
#include <fstream>
#include <unordered_map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Core/EditableMesh.h"
#include "Core/Animation.h"

namespace cm
{
	class ModelImport
	{	
	private:
		Mat4 ToMatrix4f(const aiMatrix4x4 *ai_mat);
		aiMatrix4x4 CalcRootNodeTransformMatrix(const aiNode *node);

		int32 FindMinVertexWeight(Vertex vertex);
		const aiNode* FindRootNodeOfBones(const aiNode *node, AnimationController *ac);

		
		void StoreAllBones(const aiMesh * mesh, AnimationController *ac);
		void StoreNodeTransformMatrices(const aiNode *node, AnimationController *ac);

		void SortBoneParents(const aiNode *node, uint32 parent_index, AnimationController *ac);
		void SortBoneChildren(AnimationController *ac);
		
		void ProcessAnimationChannels(aiAnimation *anim, Animation *animation);
		void ProcessBones(const aiScene *scene, AnimationController *ac);
		void ProcessAnimations(const aiScene *scene, AnimationController *ac);
		void ProcessMeshCombine(const aiScene *scene, aiMesh *mesh, uint32 index);
		void ProcessMesh(const aiScene * scene, aiNode *node,  uint32 index);

	public:
		// @TODO: Multi-thread
		bool multi_thread = false;
		// @TOOD: Multi-mesh
		bool import_mesh_combine = true;

		bool import_animations = true;

		bool import_vertex_normals = true;
		bool import_vertex_texture_coords = true;
		bool import_vertex_colours = false;
		bool import_vertex_binorms_tangents = false;
		bool vertex_boneweight_enforcement = false;
		real32 vertex_weight_cull = 0.01f;		

		std::vector<EditableMesh> resulting_meshes;
		std::vector<AnimationController> resulting_animation_controllers;
		std::vector<std::string> model_paths;

	public:
		bool Load();		
		bool Free();

	};

	class TextureImport
	{
	public:
		bool flip = false;
		std::vector<TextureConfig> texture_configs;
		std::vector<std::vector<real32>> texture_data;
		std::vector<std::string> texture_paths;

	public:
		bool Load();
		bool Free();

	};


	std::string ReadFile(const std::string &file_directory);
	
	bool LoadTexture(std::vector<uint8> *storage, TextureConfig *config, const std::string &file_directory);


	

	
}



