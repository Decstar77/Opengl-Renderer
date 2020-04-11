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
	struct ModeImport
	{		
		bool import_animations = false;
		bool import_combine = true;

		bool vertex_normals = true;
		bool vertex_colours = false;
		bool vertex_binorms_tangents = false;
		bool vertex_weight_enforcement = false;
		real32 vertex_weight_cull = 0.01f;		

		uint32 mesh_count = 0;
		EditableMesh *resulting_mesh = nullptr;

		uint32 animation_controller_count = 0;
		AnimationController *animation_controller = nullptr;

		std::string path;
	};


	std::string ReadFile(const std::string &file_directory);
	
	bool LoadTexture(DynaArray<uint8> *storage, TextureConfig *config, const std::string &file_directory);


	

	bool LoadModel(DynaArray<EditableMesh> *meshes, const std::string &path);
	bool LoadModelTest(EditableMesh *emesh, const std::string &path);
	Mat4 ToMatrix4f(const aiMatrix4x4 *ai_mat);
}



