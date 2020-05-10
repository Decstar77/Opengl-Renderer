#pragma once
#include "Core.h"

#include <filesystem>
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
		Mat4f ToMatrix4f(const aiMatrix4x4 *ai_mat);
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
		void ProcessMesh(const aiScene * scene, aiNode *node, uint32 index);

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
		std::vector<String> model_paths;

	public:
		bool Load();
		bool Free();

	};
	
	class TextureImportMultiThread
	{
	private:
		void DoLoad();

		AtomicBool done = false;
		AtomicBool working = false;

		String texture_path;
		TextureConfig texture_config;
		std::vector<uint8> texture_data;



	public:
		void Load();
		bool Free();
		bool IsLoaded();
		bool IsWorking();
		bool SetTexturePath(const String &path);

		bool flip_image = false;

		TextureConfig *GetConfig();
		std::vector<uint8> *GetData();
	};





	class TextureBank
	{
	private:
		AtomicBool staging_area_locked = false;
		std::unordered_map<uint32, Texture> current_textures;

		uint32 staging_config_next = 0;
		Array<TextureConfig> staging_configs;

		uint32 staging_data_next = 0;
		Array<Array<uint8>> staging_data;

	public:
		bool LockStagingArea();
		bool UnlockStagingArea();
		bool IsStagingAreaLocked();

		void PushTextureToStagingArea(Array<uint8> data, const TextureConfig &config);
		bool PopTextureOnStagingArea();

		bool Free(const uint32 &id);
		bool Free(const String name);

		bool Get(const uint32 &id, Texture *texture);
		bool Get(const String &name, Texture *texture);


		// @NOTE: Hard coded sizes
		void Create();
	};

	class GLMeshBank
	{
		AtomicBool staging_area_locked = false;
		std::unordered_map<uint32, GLMesh> current_mesh;
		
		uint32 staging_vertex_data_next = 0;
		Array<EditableMesh> staging_mesh_data;
	};




	String ReadTextFile(const String &file_directory);

	bool LoadTexture(std::vector<uint8> *storage, TextureConfig *config, const String &file_directory, const bool &flip);

	bool LoadTexture(Array<uint8> *storage, TextureConfig *config, const String &file_directory, const bool &flip);

	void LoadMesh(InterMesh *mesh, const String &file_directory, const bool mutli_thread = false);

	void LoadTextureFolder(TextureBank *texture_bank, const String &folder_directory, const bool &flip, const bool &multi_thread = false);
	
}



