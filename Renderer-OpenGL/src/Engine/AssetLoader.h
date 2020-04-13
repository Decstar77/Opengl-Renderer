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
	class ModeImport
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
		void ProcessMeshCombine(aiMesh *mesh, ModeImport *model_import, const aiScene *scene);
		void ProcessMesh(aiNode *node, ModeImport *model_import, const aiScene * scene);

	public:
		bool import_animations = true;
		bool import_mesh_combine = true;

		bool vertex_normals = true;
		bool vertex_texture_coords = true;
		bool vertex_colours = false;
		bool vertex_binorms_tangents = false;
		bool vertex_weight_enforcement = false;
		real32 vertex_weight_cull = 0.01f;		

		std::vector<EditableMesh> resulting_meshes;
		std::vector<AnimationController> resulting_animation_controllers;
		std::vector<std::string> model_paths;

	public:
		// @TODO: Multi-thread
		// @TOOD: Multi-mesh
		bool Load();		

	};

	class TextureImport
	{
	public:
		std::vector<TextureConfig> texture_configs;
		std::vector<std::vector<real32>> texture_data;
		std::vector<std::string> texture_paths;

	public:
		bool Load();

	};


	std::string ReadFile(const std::string &file_directory);
	
	bool LoadTexture(std::vector<uint8> *storage, TextureConfig *config, const std::string &file_directory);


	

	
}



